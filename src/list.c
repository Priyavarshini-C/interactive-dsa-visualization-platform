/* =========================================================================
 *  list.c  -  Unit-2 : List ADT
 *             Linked implementation  : singly, doubly, circular
 *             Array/cursor implementation : cursor-based list
 *             Applications : polynomial arithmetic, Josephus problem,
 *                            sparse matrix (triplet form)
 * ========================================================================= */
#include "list.h"
#include "viz.h"

/* ===================================================== SINGLY LINKED LIST */
typedef struct SNode {
    int           data;
    struct SNode *next;
} SNode;

static int  sll_flatten(SNode *h, int v[], int max)
{
    int n = 0;
    while (h && n < max) { v[n++] = h->data; h = h->next; }
    return n;
}

static void sll_show(SNode *head, int hl, const char *caption)
{
    int v[MAX_ARRAY], n;
    n = sll_flatten(head, v, MAX_ARRAY);
    if (caption) printf("\n  %s\n", caption);
    viz_chain(v, n, 0, hl, "HEAD");
    printf("    nodes = %d\n", n);
}

static SNode *sll_node(int x)
{
    SNode *p = (SNode *)xmalloc(sizeof(SNode));
    p->data = x; p->next = NULL;
    return p;
}

static SNode *sll_insert_front(SNode *head, int x)
{
    SNode *p = sll_node(x);
    p->next = head;
    return p;
}

static SNode *sll_insert_end(SNode *head, int x)
{
    SNode *p = sll_node(x), *q = head;
    if (!head) return p;
    while (q->next) q = q->next;
    q->next = p;
    return head;
}

static SNode *sll_insert_pos(SNode *head, int x, int pos)
{
    SNode *p, *q = head;
    int i;
    if (pos <= 1 || !head) return sll_insert_front(head, x);
    for (i = 1; i < pos - 1 && q->next; i++) q = q->next;
    p = sll_node(x);
    p->next = q->next;
    q->next = p;
    return head;
}

static SNode *sll_delete_value(SNode *head, int x, int *done)
{
    SNode *cur = head, *prev = NULL;
    *done = 0;
    while (cur && cur->data != x) { prev = cur; cur = cur->next; }
    if (!cur) return head;                       /* value not present */
    if (!prev) head = cur->next;
    else       prev->next = cur->next;
    free(cur);
    *done = 1;
    return head;
}

static SNode *sll_reverse(SNode *head)
{
    SNode *prev = NULL, *cur = head, *nxt;
    while (cur) { nxt = cur->next; cur->next = prev; prev = cur; cur = nxt; }
    return prev;
}

static void sll_free(SNode *h)
{
    SNode *t;
    while (h) { t = h->next; free(h); h = t; }
}

static void singly_menu(void)
{
    static const char *const items[] = {
        "Insert at beginning", "Insert at end", "Insert at position",
        "Delete by value", "Search a value (traversal shown)",
        "Reverse the list", "Display", "Load sample list 10 20 30 40"
    };
    SNode *head = NULL;
    int choice, x, pos, done, i;

    for (;;) {
        choice = ui_menu("SINGLY LINKED LIST", items, 8, "Back (frees the list)");
        if (choice == 0 || g_eof) { sll_free(head); return; }

        switch (choice) {
        case 1:
            x = ui_read_int("  Value to insert at front: ", -9999, 9999);
            sll_show(head, -1, "BEFORE");
            head = sll_insert_front(head, x);
            sll_show(head, 0, "AFTER  (new node becomes HEAD)");
            ui_note("New node's next points to the old head. O(1).");
            break;
        case 2:
            x = ui_read_int("  Value to insert at end: ", -9999, 9999);
            sll_show(head, -1, "BEFORE");
            head = sll_insert_end(head, x);
            { int v[MAX_ARRAY]; int n = sll_flatten(head, v, MAX_ARRAY);
              sll_show(head, n - 1, "AFTER  (appended at the tail)"); }
            ui_note("Needs a full traversal to reach the last node. O(n).");
            break;
        case 3:
            x   = ui_read_int("  Value to insert: ", -9999, 9999);
            pos = ui_read_int("  Position (1 = front): ", 1, 100);
            sll_show(head, -1, "BEFORE");
            head = sll_insert_pos(head, x, pos);
            sll_show(head, pos - 1, "AFTER");
            break;
        case 4:
            if (!head) { printf("\n  List is empty - nothing to delete.\n"); break; }
            x = ui_read_int("  Value to delete: ", -9999, 9999);
            sll_show(head, -1, "BEFORE");
            head = sll_delete_value(head, x, &done);
            if (done) { sll_show(head, -1, "AFTER  (node unlinked and freed)");
                        ui_note("prev->next = cur->next;  free(cur);"); }
            else      { printf("\n  Value %d is not present - list unchanged.\n", x); }
            break;
        case 5: {
            SNode *p = head;
            int idx = 0, found = -1;
            x = ui_read_int("  Value to search: ", -9999, 9999);
            while (p) {
                printf("\n  visit node %d (value %d)\n", idx, p->data);
                sll_show(head, idx, NULL);
                if (p->data == x) { found = idx; printf("    MATCH\n"); break; }
                printf("    not a match - follow the next pointer\n");
                ui_step();
                p = p->next; idx++;
            }
            if (found >= 0) printf("\n  Found %d at node index %d.\n", x, found);
            else            printf("\n  %d is not in the list (searched to NULL).\n", x);
            ui_complexity("Linked list search", "O(1)", "O(n)", "O(n)",
                          "O(1)", "No random access - must follow pointers");
            break;
        }
        case 6:
            sll_show(head, -1, "BEFORE reversal");
            head = sll_reverse(head);
            sll_show(head, -1, "AFTER reversal (pointers flipped in place)");
            ui_note("Three pointers prev/cur/next; one pass, O(n) time O(1) space.");
            break;
        case 7:
            sll_show(head, -1, "CURRENT LIST");
            break;
        case 8:
            sll_free(head); head = NULL;
            for (i = 4; i >= 1; i--) head = sll_insert_front(head, i * 10);
            sll_show(head, -1, "SAMPLE LIST LOADED");
            break;
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ===================================================== DOUBLY LINKED LIST */
typedef struct DNode {
    int           data;
    struct DNode *prev, *next;
} DNode;

static DNode *dll_insert_end(DNode *head, int x)
{
    DNode *p = (DNode *)xmalloc(sizeof(DNode)), *q = head;
    p->data = x; p->next = p->prev = NULL;
    if (!head) return p;
    while (q->next) q = q->next;
    q->next = p; p->prev = q;
    return head;
}

static DNode *dll_insert_front(DNode *head, int x)
{
    DNode *p = (DNode *)xmalloc(sizeof(DNode));
    p->data = x; p->prev = NULL; p->next = head;
    if (head) head->prev = p;
    return p;
}

static DNode *dll_delete_value(DNode *head, int x, int *done)
{
    DNode *c = head;
    *done = 0;
    while (c && c->data != x) c = c->next;
    if (!c) return head;
    if (c->prev) c->prev->next = c->next; else head = c->next;
    if (c->next) c->next->prev = c->prev;
    free(c);
    *done = 1;
    return head;
}

static void dll_show(DNode *h, int hl, const char *cap)
{
    int v[MAX_ARRAY], n = 0;
    DNode *p = h;
    while (p && n < MAX_ARRAY) { v[n++] = p->data; p = p->next; }
    if (cap) printf("\n  %s\n", cap);
    viz_chain(v, n, 1, hl, "HEAD");
    printf("    nodes = %d   (each node carries prev and next pointers)\n", n);
}

static void dll_free(DNode *h) { DNode *t; while (h) { t = h->next; free(h); h = t; } }

static void doubly_menu(void)
{
    static const char *const items[] = {
        "Insert at beginning", "Insert at end", "Delete by value",
        "Forward traversal", "Backward traversal", "Display",
        "Load sample list 10 20 30 40"
    };
    DNode *head = NULL, *p;
    int choice, x, done, i;

    for (;;) {
        choice = ui_menu("DOUBLY LINKED LIST", items, 7, "Back (frees the list)");
        if (choice == 0 || g_eof) { dll_free(head); return; }
        switch (choice) {
        case 1: x = ui_read_int("  Value: ", -9999, 9999);
                dll_show(head, -1, "BEFORE");
                head = dll_insert_front(head, x);
                dll_show(head, 0, "AFTER"); break;
        case 2: x = ui_read_int("  Value: ", -9999, 9999);
                dll_show(head, -1, "BEFORE");
                head = dll_insert_end(head, x);
                dll_show(head, -1, "AFTER"); break;
        case 3: if (!head) { printf("\n  Empty list.\n"); break; }
                x = ui_read_int("  Value to delete: ", -9999, 9999);
                dll_show(head, -1, "BEFORE");
                head = dll_delete_value(head, x, &done);
                if (done) { dll_show(head, -1, "AFTER");
                            ui_note("Both neighbour links are repaired, then free()."); }
                else printf("\n  %d not found.\n", x);
                break;
        case 4: printf("\n  Forward  : ");
                for (p = head; p; p = p->next) printf("%d ", p->data);
                printf("\n"); break;
        case 5: if (!head) { printf("\n  Empty list.\n"); break; }
                for (p = head; p->next; p = p->next) ;
                printf("\n  Backward : ");
                for (; p; p = p->prev) printf("%d ", p->data);
                printf("\n  (only possible because of the prev pointer)\n");
                break;
        case 6: dll_show(head, -1, "CURRENT LIST"); break;
        case 7: dll_free(head); head = NULL;
                for (i = 1; i <= 4; i++) head = dll_insert_end(head, i * 10);
                dll_show(head, -1, "SAMPLE LIST LOADED"); break;
        default: break;
        }
        ui_pause(NULL);
    }
}

/* =================================================== CIRCULAR LINKED LIST */
static void circular_menu(void)
{
    static const char *const items[] = {
        "Insert at end", "Delete by value", "Traverse one full round",
        "Display", "Load sample list 10 20 30 40"
    };
    SNode *last = NULL;          /* keep a pointer to the LAST node */
    int choice, x, i;

    for (;;) {
        int v[MAX_ARRAY], n = 0;
        SNode *p;
        choice = ui_menu("CIRCULAR LINKED LIST", items, 5, "Back (frees the list)");
        if (choice == 0 || g_eof) {
            if (last) { SNode *h = last->next, *t; last->next = NULL;
                        while (h) { t = h->next; free(h); h = t; } }
            return;
        }
        switch (choice) {
        case 1:
            x = ui_read_int("  Value: ", -9999, 9999);
            { SNode *q = sll_node(x);
              if (!last) { last = q; q->next = q; }
              else { q->next = last->next; last->next = q; last = q; } }
            break;
        case 2:
            if (!last) { printf("\n  Empty list.\n"); break; }
            x = ui_read_int("  Value to delete: ", -9999, 9999);
            { SNode *cur = last->next, *prev = last;
              int found = 0, guard = 0;
              do {
                  if (cur->data == x) { found = 1; break; }
                  prev = cur; cur = cur->next; guard++;
              } while (cur != last->next && guard < 1000);
              if (!found) printf("\n  %d not found.\n", x);
              else if (cur == cur->next) { free(cur); last = NULL; }
              else { prev->next = cur->next;
                     if (cur == last) last = prev;
                     free(cur); }
            }
            break;
        case 3:
            if (!last) { printf("\n  Empty list.\n"); break; }
            p = last->next;
            printf("\n  Starting at HEAD and following next until we come back:\n");
            do {
                printf("    visit %d\n", p->data);
                p = p->next;
            } while (p != last->next);
            printf("    ... back at HEAD - the list is circular.\n");
            break;
        case 5:
            if (last) { SNode *h = last->next, *t; last->next = NULL;
                        while (h) { t = h->next; free(h); h = t; } last = NULL; }
            for (i = 1; i <= 4; i++) {
                SNode *q = sll_node(i * 10);
                if (!last) { last = q; q->next = q; }
                else { q->next = last->next; last->next = q; last = q; }
            }
            break;
        default: break;
        }
        if (last) {
            p = last->next;
            do { v[n++] = p->data; p = p->next; } while (p != last->next && n < MAX_ARRAY);
        }
        printf("\n  CURRENT LIST\n");
        viz_chain(v, n, 2, -1, "HEAD");
        printf("    nodes = %d   (last->next points back to the head)\n", n);
        ui_pause(NULL);
    }
}

/* ======================================================= CURSOR-BASED LIST */
/* Array implementation of a linked list: "pointers" are array indices.      */
#define CURSOR_MAX 12
typedef struct { int data, next; } CNode;

static CNode cspace[CURSOR_MAX];
static int   cfree_head, clist_head;

static void cursor_init(void)
{
    int i;
    for (i = 0; i < CURSOR_MAX - 1; i++) { cspace[i].data = 0; cspace[i].next = i + 1; }
    cspace[CURSOR_MAX - 1].data = 0;
    cspace[CURSOR_MAX - 1].next = -1;
    cfree_head = 0;
    clist_head = -1;
}

static int cursor_alloc(void)
{
    int p = cfree_head;
    if (p == -1) return -1;
    cfree_head = cspace[p].next;
    cspace[p].next = -1;
    return p;
}

static void cursor_release(int p)
{
    cspace[p].next = cfree_head;
    cfree_head = p;
}

static void cursor_dump(int hl)
{
    int i, p, n = 0;
    printf("\n  CURSOR SPACE (the whole array, including the free list)\n");
    printf("  +-------+--------+--------+\n");
    printf("  | index |  data  |  next  |\n");
    printf("  +-------+--------+--------+\n");
    for (i = 0; i < CURSOR_MAX; i++)
        printf("  | %s%3d%s |  %5d |  %5d |\n",
               i == hl ? ">" : " ", i, i == hl ? "<" : " ",
               cspace[i].data, cspace[i].next);
    printf("  +-------+--------+--------+\n");
    printf("    list head = %d        free head = %d   (-1 means NULL)\n",
           clist_head, cfree_head);
    printf("\n  LOGICAL LIST : HEAD");
    for (p = clist_head; p != -1 && n < CURSOR_MAX; p = cspace[p].next, n++)
        printf(" -> [%d @%d]", cspace[p].data, p);
    printf(" -> NULL\n");
}

static void cursor_menu(void)
{
    static const char *const items[] = {
        "Insert at end", "Delete by value", "Show cursor space + logical list"
    };
    int choice, x;

    cursor_init();
    for (;;) {
        choice = ui_menu("CURSOR-BASED LIST (array implementation)",
                         items, 3, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: {
            int p;
            x = ui_read_int("  Value: ", -9999, 9999);
            p = cursor_alloc();
            if (p == -1) { printf("\n  !! Cursor space exhausted (array full).\n"); break; }
            cspace[p].data = x;
            cspace[p].next = -1;
            if (clist_head == -1) clist_head = p;
            else { int q = clist_head;
                   while (cspace[q].next != -1) q = cspace[q].next;
                   cspace[q].next = p; }
            printf("\n  cursor_alloc() took cell %d from the free list.\n", p);
            cursor_dump(p);
            break;
        }
        case 2: {
            int p = clist_head, prev = -1;
            if (clist_head == -1) { printf("\n  List is empty.\n"); break; }
            x = ui_read_int("  Value to delete: ", -9999, 9999);
            while (p != -1 && cspace[p].data != x) { prev = p; p = cspace[p].next; }
            if (p == -1) { printf("\n  %d not found.\n", x); break; }
            if (prev == -1) clist_head = cspace[p].next;
            else cspace[prev].next = cspace[p].next;
            cursor_release(p);
            printf("\n  cell %d returned to the free list.\n", p);
            cursor_dump(p);
            break;
        }
        case 3: cursor_dump(-1); break;
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ================================================ APPLICATION : POLYNOMIAL */
typedef struct PNode {
    int coef, expo;
    struct PNode *next;
} PNode;

static PNode *poly_insert(PNode *head, int c, int e)
{
    PNode *p, *cur = head, *prev = NULL;
    if (c == 0) return head;
    while (cur && cur->expo > e) { prev = cur; cur = cur->next; }
    if (cur && cur->expo == e) {
        cur->coef += c;
        if (cur->coef == 0) {                     /* term cancelled out */
            if (prev) prev->next = cur->next; else head = cur->next;
            free(cur);
        }
        return head;
    }
    p = (PNode *)xmalloc(sizeof(PNode));
    p->coef = c; p->expo = e; p->next = cur;
    if (prev) prev->next = p; else head = p;
    return head;
}

static void poly_print(const char *name, PNode *h)
{
    int first = 1, c;
    printf("  %s = ", name);
    if (!h) { printf("0\n"); return; }
    while (h) {
        c = h->coef;
        if (first) { if (c < 0) { printf("-"); c = -c; } }
        else { printf(" %s ", c < 0 ? "-" : "+"); if (c < 0) c = -c; }
        if (h->expo == 0)       printf("%d", c);
        else if (h->expo == 1)  { if (c == 1) printf("x"); else printf("%dx", c); }
        else                    { if (c == 1) printf("x^%d", h->expo);
                                  else printf("%dx^%d", c, h->expo); }
        first = 0;
        h = h->next;
    }
    printf("\n");
}

static void poly_free(PNode *h) { PNode *t; while (h) { t = h->next; free(h); h = t; } }

static void polynomial_demo(void)
{
    PNode *A = NULL, *B = NULL, *C = NULL, *p;
    int n, i, c, e;

    ui_header("APPLICATION : POLYNOMIAL ADDITION USING LINKED LISTS");
    ui_note("Each node stores one term (coefficient, exponent).");
    ui_note("Terms are kept in descending order of exponent.");

    n = ui_read_int("\n  Number of terms in polynomial A: ", 1, 8);
    for (i = 0; i < n; i++) {
        c = ui_read_int("    coefficient: ", -999, 999);
        e = ui_read_int("    exponent   : ", 0, 20);
        A = poly_insert(A, c, e);
    }
    n = ui_read_int("\n  Number of terms in polynomial B: ", 1, 8);
    for (i = 0; i < n; i++) {
        c = ui_read_int("    coefficient: ", -999, 999);
        e = ui_read_int("    exponent   : ", 0, 20);
        B = poly_insert(B, c, e);
    }

    printf("\n");
    poly_print("A", A);
    poly_print("B", B);

    printf("\n  Adding term by term:\n");
    for (p = A; p; p = p->next) {
        printf("    take %dx^%d from A\n", p->coef, p->expo);
        C = poly_insert(C, p->coef, p->expo);
    }
    for (p = B; p; p = p->next) {
        printf("    take %dx^%d from B  (merged into the matching exponent)\n",
               p->coef, p->expo);
        C = poly_insert(C, p->coef, p->expo);
    }
    printf("\n");
    poly_print("A + B", C);
    ui_complexity("Polynomial addition (linked)", "O(m+n)", "O(m+n)", "O(m*n) naive insert",
                  "O(m+n) for the result list", "Sorted-by-exponent merge");
    poly_free(A); poly_free(B); poly_free(C);
    ui_pause(NULL);
}

/* =================================================== APPLICATION : JOSEPHUS */
static void josephus_demo(void)
{
    SNode *last = NULL, *cur, *prev;
    int n, k, i, alive, v[MAX_ARRAY], cnt;

    ui_header("APPLICATION : JOSEPHUS PROBLEM (circular list elimination)");
    n = ui_read_int("\n  Number of people standing in the circle (n): ", 1, 20);
    k = ui_read_int("  Every k-th person is eliminated (k): ", 1, 20);

    for (i = 1; i <= n; i++) {
        SNode *q = sll_node(i);
        if (!last) { last = q; q->next = q; }
        else { q->next = last->next; last->next = q; last = q; }
    }
    cnt = 0;
    cur = last->next;
    do { v[cnt++] = cur->data; cur = cur->next; } while (cur != last->next && cnt < MAX_ARRAY);
    printf("\n  Initial circle:\n");
    viz_chain(v, cnt, 2, -1, "START");

    prev  = last;
    cur   = last->next;
    alive = n;
    while (alive > 1) {
        for (i = 1; i < k; i++) { prev = cur; cur = cur->next; }
        printf("\n  count %d -> person %d is eliminated\n", k, cur->data);
        prev->next = cur->next;
        if (cur == last) last = prev;
        free(cur);
        cur = prev->next;
        alive--;

        cnt = 0;
        { SNode *p = cur;
          do { v[cnt++] = p->data; p = p->next; } while (p != cur && cnt < MAX_ARRAY); }
        viz_chain(v, cnt, 2, -1, "REMAINING");
        ui_step();
    }
    printf("\n  SURVIVOR : person %d\n", cur->data);
    free(cur);
    ui_complexity("Josephus (circular list)", "O(n*k)", "O(n*k)", "O(n*k)",
                  "O(n) for the circle", "Deletion from a circular linked list");
    ui_pause(NULL);
}

/* ============================================= APPLICATION : SPARSE MATRIX */
static void sparse_demo(void)
{
    int m, n, i, j, nz = 0, val;
    int mat[8][8];
    int tr[64][3];

    ui_header("APPLICATION : SPARSE MATRIX (triplet representation)");
    m = ui_read_int("\n  Rows (<=8): ", 1, 8);
    n = ui_read_int("  Columns (<=8): ", 1, 8);
    printf("  Enter the matrix row by row (mostly zeros is the interesting case):\n");
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++) {
            char p[48];
            sprintf(p, "    a[%d][%d] = ", i, j);
            val = ui_read_int(p, -999, 999);
            mat[i][j] = val;
            if (val != 0) { tr[nz][0] = i; tr[nz][1] = j; tr[nz][2] = val; nz++; }
        }

    printf("\n  FULL MATRIX (%d x %d = %d cells)\n", m, n, m * n);
    printf("  +");
    for (j = 0; j < n; j++) printf("------");
    printf("+\n");
    for (i = 0; i < m; i++) {
        printf("  |");
        for (j = 0; j < n; j++) printf("%5d ", mat[i][j]);
        printf("|\n");
    }
    printf("  +");
    for (j = 0; j < n; j++) printf("------");
    printf("+\n");

    printf("\n  TRIPLET FORM  (row, column, value)\n");
    printf("  +-------+--------+--------+\n");
    printf("  |  row  |  col   | value  |\n");
    printf("  +-------+--------+--------+\n");
    printf("  | %5d | %6d | %6d |   <- header: dimensions and count\n", m, n, nz);
    for (i = 0; i < nz; i++)
        printf("  | %5d | %6d | %6d |\n", tr[i][0], tr[i][1], tr[i][2]);
    printf("  +-------+--------+--------+\n");
    printf("\n  Storage: %d cells -> %d triplet rows.\n", m * n, nz + 1);
    if (3 * (nz + 1) < m * n) printf("  The sparse form saves space here.\n");
    else printf("  This matrix is too dense for the triplet form to pay off.\n");
    ui_pause(NULL);
}

/* ================================================================== menu */
void list_menu(void)
{
    static const char *const items[] = {
        "Singly Linked List",
        "Doubly Linked List",
        "Circular Linked List",
        "Cursor-based List (array implementation)",
        "Application : Polynomial addition",
        "Application : Josephus problem",
        "Application : Sparse matrix (triplet form)"
    };
    int choice;
    for (;;) {
        choice = ui_menu("LIST STRUCTURES  (Unit 2)", items, 7, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: singly_menu();     break;
        case 2: doubly_menu();     break;
        case 3: circular_menu();   break;
        case 4: cursor_menu();     break;
        case 5: polynomial_demo(); break;
        case 6: josephus_demo();   break;
        case 7: sparse_demo();     break;
        default: break;
        }
    }
}
