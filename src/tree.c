/* =========================================================================
 *  tree.c  -  Unit-4 : Trees
 *             Binary Search Tree : insert, delete (3 cases), search,
 *                                  inorder / preorder / postorder / level order,
 *                                  height, completeness check, array form
 *             AVL Tree           : insert with LL, RR, LR, RL rotations
 *             B-Tree             : insert with node splitting, search, display
 * ========================================================================= */
#include "tree.h"
#include "viz.h"

/* ========================================================================= */
/*                            BINARY SEARCH TREE                             */
/* ========================================================================= */
typedef struct TNode {
    int           data;
    int           height;            /* used by the AVL variant */
    struct TNode *l, *r;
} TNode;

static TNode *tnode(int x)
{
    TNode *p = (TNode *)xmalloc(sizeof(TNode));
    p->data = x; p->height = 1; p->l = p->r = NULL;
    return p;
}

static void tfree(TNode *t) { if (!t) return; tfree(t->l); tfree(t->r); free(t); }

static int theight(TNode *t)
{
    int hl, hr;
    if (!t) return 0;
    hl = theight(t->l); hr = theight(t->r);
    return 1 + (hl > hr ? hl : hr);
}

static int tcount(TNode *t) { return t ? 1 + tcount(t->l) + tcount(t->r) : 0; }
static int tleaves(TNode *t)
{
    if (!t) return 0;
    if (!t->l && !t->r) return 1;
    return tleaves(t->l) + tleaves(t->r);
}

/* ---- mirror into the generic renderer ---------------------------------- */
static VTree *mirror(TNode *t, int emph_val, int use_emph)
{
    char buf[20];
    VTree *v;
    if (!t) return NULL;
    sprintf(buf, "%d", t->data);
    v = vt_new(buf, use_emph && t->data == emph_val);
    v->l = mirror(t->l, emph_val, use_emph);
    v->r = mirror(t->r, emph_val, use_emph);
    return v;
}

static void show_tree(TNode *root, int emph_val, int use_emph, const char *cap)
{
    VTree *v;
    if (cap) printf("\n  %s\n", cap);
    if (!root) { printf("\n    ( tree is empty )\n"); return; }
    v = mirror(root, emph_val, use_emph);
    viz_tree(v);
    vt_free(v);
    printf("\n    nodes = %d   height = %d   leaves = %d\n",
           tcount(root), theight(root), tleaves(root));
}

/* ---- insert ------------------------------------------------------------ */
static TNode *bst_insert(TNode *t, int x, int verbose, int depth)
{
    int i;
    if (!t) {
        if (verbose) { for (i = 0; i < depth; i++) printf("  ");
                       printf("    reached an empty spot -> create node %d\n", x); }
        return tnode(x);
    }
    if (verbose) {
        for (i = 0; i < depth; i++) printf("  ");
        printf("    at node %d : ", t->data);
    }
    if (x < t->data) {
        if (verbose) printf("%d < %d -> go LEFT\n", x, t->data);
        t->l = bst_insert(t->l, x, verbose, depth + 1);
    } else if (x > t->data) {
        if (verbose) printf("%d > %d -> go RIGHT\n", x, t->data);
        t->r = bst_insert(t->r, x, verbose, depth + 1);
    } else {
        if (verbose) printf("%d already present -> duplicates are rejected\n", x);
    }
    return t;
}

/* ---- search ------------------------------------------------------------ */
static TNode *bst_search(TNode *t, int x, int verbose, int *steps)
{
    while (t) {
        (*steps)++;
        if (verbose) printf("    compare %d with node %d : ", x, t->data);
        if (x == t->data) { if (verbose) printf("MATCH\n"); return t; }
        if (x < t->data) { if (verbose) printf("smaller -> left subtree\n");  t = t->l; }
        else             { if (verbose) printf("greater -> right subtree\n"); t = t->r; }
    }
    if (verbose) printf("    reached NULL -> value is not in the tree\n");
    return NULL;
}

/* ---- delete ------------------------------------------------------------ */
static TNode *bst_min(TNode *t) { while (t && t->l) t = t->l; return t; }

static TNode *bst_delete(TNode *t, int x, int verbose)
{
    if (!t) { if (verbose) printf("    %d not found - nothing deleted\n", x);
              return NULL; }
    if (x < t->data)      t->l = bst_delete(t->l, x, verbose);
    else if (x > t->data) t->r = bst_delete(t->r, x, verbose);
    else {
        if (!t->l && !t->r) {
            if (verbose) printf("    CASE 1 : %d is a LEAF -> free it directly\n", x);
            free(t); return NULL;
        }
        if (!t->l || !t->r) {
            TNode *c = t->l ? t->l : t->r;
            if (verbose) printf("    CASE 2 : %d has ONE child (%d) -> the child "
                                "replaces it\n", x, c->data);
            free(t); return c;
        }
        {
            TNode *s = bst_min(t->r);
            if (verbose) printf("    CASE 3 : %d has TWO children -> copy the "
                                "inorder successor %d, then delete %d from the "
                                "right subtree\n", x, s->data, s->data);
            t->data = s->data;
            t->r    = bst_delete(t->r, s->data, 0);
        }
    }
    return t;
}

/* ---- traversals -------------------------------------------------------- */
static void t_inorder(TNode *t, int out[], int *n)
{ if (!t) return; t_inorder(t->l, out, n); out[(*n)++] = t->data; t_inorder(t->r, out, n); }
static void t_preorder(TNode *t, int out[], int *n)
{ if (!t) return; out[(*n)++] = t->data; t_preorder(t->l, out, n); t_preorder(t->r, out, n); }
static void t_postorder(TNode *t, int out[], int *n)
{ if (!t) return; t_postorder(t->l, out, n); t_postorder(t->r, out, n); out[(*n)++] = t->data; }

static void t_levelorder(TNode *t, int out[], int *n)
{
    TNode *q[256];
    int head = 0, tail = 0;
    *n = 0;
    if (!t) return;
    q[tail++] = t;
    while (head < tail) {
        TNode *c = q[head++];
        out[(*n)++] = c->data;
        if (c->l) q[tail++] = c->l;
        if (c->r) q[tail++] = c->r;
    }
}

static void show_sequence(const char *name, const int v[], int n)
{
    int i;
    printf("    %-12s : ", name);
    for (i = 0; i < n; i++) printf("%d%s", v[i], i < n - 1 ? " -> " : "");
    printf("\n");
}

static void traversal_demo(TNode *root)
{
    int v[256], n = 0, i;
    if (!root) { printf("\n  Tree is empty.\n"); return; }

    show_tree(root, 0, 0, "CURRENT TREE");

    ui_header("INORDER  (Left, Root, Right)");
    n = 0; t_inorder(root, v, &n);
    printf("\n  Visiting order built by the recursion:\n");
    show_sequence("Inorder", v, n);
    ui_note("For a BST the inorder traversal always comes out SORTED.");
    ui_step();

    ui_header("PREORDER  (Root, Left, Right)");
    n = 0; t_preorder(root, v, &n);
    show_sequence("Preorder", v, n);
    ui_note("Useful to copy/serialise a tree - the root is emitted first.");
    ui_step();

    ui_header("POSTORDER  (Left, Right, Root)");
    n = 0; t_postorder(root, v, &n);
    show_sequence("Postorder", v, n);
    ui_note("Useful to free a tree - children are released before the parent.");
    ui_step();

    ui_header("LEVEL ORDER  (Breadth First, uses a queue)");
    n = 0; t_levelorder(root, v, &n);
    show_sequence("Level order", v, n);
    printf("\n  Level by level:\n");
    {
        TNode *q[256], *lvl[256];
        int head = 0, tail = 0, depth = 0;
        q[tail++] = root;
        while (head < tail) {
            int cnt = tail - head, k;
            for (k = 0; k < cnt; k++) lvl[k] = q[head + k];
            printf("    level %d : ", depth);
            for (k = 0; k < cnt; k++) printf("%d ", lvl[k]->data);
            printf("\n");
            for (k = 0; k < cnt; k++) {
                TNode *c = q[head++];
                if (c->l) q[tail++] = c->l;
                if (c->r) q[tail++] = c->r;
            }
            depth++;
        }
    }
    ui_complexity("Tree traversal", "O(n)", "O(n)", "O(n)",
                  "O(h) recursion stack / O(w) queue for level order",
                  "Every node is visited exactly once");
    (void)i;
}

/* ---- completeness and array representation ----------------------------- */
static void completeness_demo(TNode *root)
{
    TNode *q[256];
    int head = 0, tail = 0, seen_null = 0, complete = 1, i;
    int arr[256], used[256], maxidx = -1;

    if (!root) { printf("\n  Tree is empty.\n"); return; }
    show_tree(root, 0, 0, "CURRENT TREE");

    q[tail++] = root;
    while (head < tail) {
        TNode *c = q[head++];
        if (!c) { seen_null = 1; continue; }
        if (seen_null) complete = 0;
        q[tail++] = c->l;
        q[tail++] = c->r;
    }
    printf("\n  Level-order scan with NULL children included:\n");
    printf("  If a NULL appears before any real node, the tree is NOT complete.\n");
    printf("\n  RESULT : the tree is %sa COMPLETE binary tree.\n",
           complete ? "" : "NOT ");
    printf("  Height h = %d, node count n = %d\n", theight(root), tcount(root));
    printf("  A complete binary tree with n nodes has height ceil(log2(n+1)).\n");
    {
        int n = tcount(root), h = 0, p = 1;
        while (p < n + 1) { p *= 2; h++; }
        printf("  For n = %d that formula gives h = %d.\n", n, h);
    }

    /* array representation: index i -> children 2i+1, 2i+2 */
    for (i = 0; i < 256; i++) used[i] = 0;
    {
        TNode *stack[256];
        int idx[256], sp = 0;
        stack[sp] = root; idx[sp] = 0; sp++;
        while (sp) {
            TNode *c = stack[--sp];
            int k = idx[sp];
            if (k < 256) { arr[k] = c->data; used[k] = 1; if (k > maxidx) maxidx = k; }
            if (c->r && 2 * k + 2 < 256) { stack[sp] = c->r; idx[sp] = 2 * k + 2; sp++; }
            if (c->l && 2 * k + 1 < 256) { stack[sp] = c->l; idx[sp] = 2 * k + 1; sp++; }
        }
    }
    printf("\n  ARRAY REPRESENTATION (root at index 0, children at 2i+1 and 2i+2)\n");
    printf("  +-------+--------+\n  | index | value  |\n  +-------+--------+\n");
    for (i = 0; i <= maxidx && i < 31; i++) {
        if (used[i]) printf("  | %5d | %6d |\n", i, arr[i]);
        else         printf("  | %5d | %6s |   <- empty slot\n", i, "-");
    }
    printf("  +-------+--------+\n");
    if (!complete)
        printf("  Gaps above are exactly why an array is a poor fit for a "
               "non-complete tree.\n");
}

/* ---- BST menu ---------------------------------------------------------- */
static void bst_menu(void)
{
    static const char *const items[] = {
        "Insert a value", "Delete a value", "Search a value",
        "All four traversals", "Find minimum and maximum",
        "Height / node count / leaf count",
        "Completeness check + array representation",
        "Load sample tree  50 30 70 20 40 60 80", "Display tree"
    };
    TNode *root = NULL;
    int choice, x, i;
    int sample[7] = { 50, 30, 70, 20, 40, 60, 80 };

    for (;;) {
        choice = ui_menu("BINARY SEARCH TREE", items, 9, "Back (frees the tree)");
        if (choice == 0 || g_eof) { tfree(root); return; }
        switch (choice) {
        case 1:
            x = ui_read_int("  Value to insert: ", -9999, 9999);
            show_tree(root, 0, 0, "BEFORE");
            printf("\n  Walking down from the root:\n");
            root = bst_insert(root, x, 1, 0);
            show_tree(root, x, 1, "AFTER  (*value* marks the new node)");
            ui_complexity("BST insert", "O(log n) balanced", "O(log n)",
                          "O(n) degenerate (sorted input)",
                          "O(h) recursion stack", "No duplicates stored");
            break;
        case 2:
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            x = ui_read_int("  Value to delete: ", -9999, 9999);
            show_tree(root, x, 1, "BEFORE");
            { int steps = 0;
              if (!bst_search(root, x, 0, &steps))
                  printf("\n  %d is not in the tree - nothing to delete.\n", x);
              else { printf("\n  Deletion:\n");
                     root = bst_delete(root, x, 1);
                     show_tree(root, 0, 0, "AFTER"); } }
            break;
        case 3: {
            int steps = 0;
            TNode *f;
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            x = ui_read_int("  Value to search: ", -9999, 9999);
            printf("\n  Search path:\n");
            f = bst_search(root, x, 1, &steps);
            show_tree(root, x, f != NULL, f ? "FOUND (*marked*)" : "NOT FOUND");
            printf("\n  Comparisons made : %d   (tree height = %d)\n",
                   steps, theight(root));
            ui_complexity("BST search", "O(1)", "O(log n)", "O(n) degenerate",
                          "O(1) iterative", "Halves the search space at each node");
            break;
        }
        case 4: traversal_demo(root); break;
        case 5:
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            { TNode *mn = root, *mx = root;
              while (mn->l) mn = mn->l;
              while (mx->r) mx = mx->r;
              printf("\n  Minimum = %d  (leftmost node)\n", mn->data);
              printf("  Maximum = %d  (rightmost node)\n", mx->data);
              show_tree(root, mn->data, 1, "MINIMUM MARKED"); }
            break;
        case 6:
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            show_tree(root, 0, 0, "CURRENT TREE");
            printf("\n  height (number of levels) = %d\n", theight(root));
            printf("  total nodes               = %d\n", tcount(root));
            printf("  leaf nodes                = %d\n", tleaves(root));
            printf("  internal nodes            = %d\n",
                   tcount(root) - tleaves(root));
            break;
        case 7: completeness_demo(root); break;
        case 8:
            tfree(root); root = NULL;
            for (i = 0; i < 7; i++) root = bst_insert(root, sample[i], 0, 0);
            show_tree(root, 0, 0, "SAMPLE TREE LOADED");
            break;
        case 9: show_tree(root, 0, 0, "CURRENT TREE"); break;
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ========================================================================= */
/*                                AVL TREE                                   */
/* ========================================================================= */
static int avl_h(TNode *t) { return t ? t->height : 0; }
static int avl_max(int a, int b) { return a > b ? a : b; }
static int avl_bf(TNode *t) { return t ? avl_h(t->l) - avl_h(t->r) : 0; }

static TNode *rotate_right(TNode *y)
{
    TNode *x = y->l, *t2 = x->r;
    x->r = y; y->l = t2;
    y->height = 1 + avl_max(avl_h(y->l), avl_h(y->r));
    x->height = 1 + avl_max(avl_h(x->l), avl_h(x->r));
    return x;
}

static TNode *rotate_left(TNode *x)
{
    TNode *y = x->r, *t2 = y->l;
    y->l = x; x->r = t2;
    x->height = 1 + avl_max(avl_h(x->l), avl_h(x->r));
    y->height = 1 + avl_max(avl_h(y->l), avl_h(y->r));
    return y;
}

static void avl_print_bf(TNode *t, int depth)
{
    int i;
    if (!t) return;
    avl_print_bf(t->l, depth + 1);
    for (i = 0; i < depth; i++) printf("  ");
    printf("    node %-4d height=%d  balance factor=%+d %s\n",
           t->data, t->height, avl_bf(t),
           (avl_bf(t) > 1 || avl_bf(t) < -1) ? "  <-- UNBALANCED" : "");
    avl_print_bf(t->r, depth + 1);
}

static TNode *avl_insert(TNode *t, int x, int verbose)
{
    int bf;
    if (!t) return tnode(x);
    if      (x < t->data) t->l = avl_insert(t->l, x, verbose);
    else if (x > t->data) t->r = avl_insert(t->r, x, verbose);
    else return t;

    t->height = 1 + avl_max(avl_h(t->l), avl_h(t->r));
    bf = avl_bf(t);

    if (bf > 1 && x < t->l->data) {
        if (verbose) printf("    node %d has balance factor %+d and the new key "
                            "went LEFT-LEFT -> single RIGHT rotation (LL)\n", t->data, bf);
        return rotate_right(t);
    }
    if (bf < -1 && x > t->r->data) {
        if (verbose) printf("    node %d has balance factor %+d and the new key "
                            "went RIGHT-RIGHT -> single LEFT rotation (RR)\n", t->data, bf);
        return rotate_left(t);
    }
    if (bf > 1 && x > t->l->data) {
        if (verbose) printf("    node %d has balance factor %+d, LEFT-RIGHT case "
                            "-> left rotate the child, then right rotate (LR)\n",
                            t->data, bf);
        t->l = rotate_left(t->l);
        return rotate_right(t);
    }
    if (bf < -1 && x < t->r->data) {
        if (verbose) printf("    node %d has balance factor %+d, RIGHT-LEFT case "
                            "-> right rotate the child, then left rotate (RL)\n",
                            t->data, bf);
        t->r = rotate_right(t->r);
        return rotate_left(t);
    }
    return t;
}

static void avl_menu(void)
{
    static const char *const items[] = {
        "Insert a value (rotations explained)",
        "Show balance factors",
        "Display tree",
        "Demo : insert 50 40 30 60 70 10 20 55 52 (all 4 rotation types)",
        "Compare with an unbalanced BST on the same keys"
    };
    TNode *root = NULL;
    int choice, x, i;

    for (;;) {
        choice = ui_menu("AVL TREE (height balanced BST)", items, 5,
                         "Back (frees the tree)");
        if (choice == 0 || g_eof) { tfree(root); return; }
        switch (choice) {
        case 1:
            x = ui_read_int("  Value to insert: ", -9999, 9999);
            show_tree(root, 0, 0, "BEFORE");
            printf("\n  Inserting %d and re-balancing on the way back up:\n", x);
            root = avl_insert(root, x, 1);
            show_tree(root, x, 1, "AFTER");
            break;
        case 2:
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            show_tree(root, 0, 0, "CURRENT TREE");
            printf("\n  balance factor = height(left) - height(right)\n");
            printf("  An AVL tree keeps every balance factor in {-1, 0, +1}.\n\n");
            avl_print_bf(root, 0);
            break;
        case 3: show_tree(root, 0, 0, "CURRENT TREE"); break;
        case 4: {
            int keys[9] = { 50, 40, 30, 60, 70, 10, 20, 55, 52 };
            tfree(root); root = NULL;
            printf("\n  This key order deliberately triggers all four rebalancing\n");
            printf("  cases: LL, RR, LR and RL.\n");
            for (i = 0; i < 9; i++) {
                printf("\n  ---- inserting %d ----\n", keys[i]);
                root = avl_insert(root, keys[i], 1);
                show_tree(root, keys[i], 1, NULL);
                ui_step();
            }
            printf("\n  %d keys stored with height %d. A plain BST on badly ordered\n",
                   9, theight(root));
            printf("  keys can reach height %d instead.\n", 9);
            break;
        }
        case 5: {
            int keys[6] = { 10, 20, 30, 40, 50, 60 };
            TNode *plain = NULL, *avl = NULL;
            for (i = 0; i < 6; i++) {
                plain = bst_insert(plain, keys[i], 0, 0);
                avl   = avl_insert(avl, keys[i], 0);
            }
            printf("\n  Same ascending keys 10 20 30 40 50 60 inserted into both:\n");
            show_tree(plain, 0, 0, "PLAIN BST  (degenerates - height 6)");
            show_tree(avl, 0, 0, "AVL TREE   (stays balanced)");
            printf("\n  Search cost in the plain BST : up to %d comparisons\n",
                   theight(plain));
            printf("  Search cost in the AVL tree  : up to %d comparisons\n",
                   theight(avl));
            ui_note("This is exactly why the syllabus asks for 'need for balance'.");
            tfree(plain); tfree(avl);
            break;
        }
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ========================================================================= */
/*                                 B-TREE                                    */
/* ========================================================================= */
#define BT_T    2                      /* minimum degree: 1..3 keys per node */
#define BT_MAXK (2 * BT_T - 1)
#define BT_MAXC (2 * BT_T)

typedef struct BNode {
    int           n;
    int           key[BT_MAXK];
    struct BNode *c[BT_MAXC];
    int           leaf;
} BNode;

static BNode *bt_new(int leaf)
{
    BNode *x = (BNode *)xmalloc(sizeof(BNode));
    int i;
    x->n = 0; x->leaf = leaf;
    for (i = 0; i < BT_MAXC; i++) x->c[i] = NULL;
    return x;
}

static void bt_free(BNode *x)
{
    int i;
    if (!x) return;
    if (!x->leaf) for (i = 0; i <= x->n; i++) bt_free(x->c[i]);
    free(x);
}

static void bt_split_child(BNode *x, int i, int verbose)
{
    BNode *y = x->c[i];
    BNode *z = bt_new(y->leaf);
    int j;

    if (verbose)
        printf("    node is FULL (%d keys) -> split it: middle key %d moves up\n",
               y->n, y->key[BT_T - 1]);

    z->n = BT_T - 1;
    for (j = 0; j < BT_T - 1; j++) z->key[j] = y->key[j + BT_T];
    if (!y->leaf) for (j = 0; j < BT_T; j++) z->c[j] = y->c[j + BT_T];
    y->n = BT_T - 1;

    for (j = x->n; j >= i + 1; j--) x->c[j + 1] = x->c[j];
    x->c[i + 1] = z;
    for (j = x->n - 1; j >= i; j--) x->key[j + 1] = x->key[j];
    x->key[i] = y->key[BT_T - 1];
    x->n++;
}

static void bt_insert_nonfull(BNode *x, int k, int verbose)
{
    int i = x->n - 1;
    if (x->leaf) {
        while (i >= 0 && x->key[i] > k) { x->key[i + 1] = x->key[i]; i--; }
        x->key[i + 1] = k;
        x->n++;
        if (verbose) printf("    placed %d in a leaf (now holds %d key(s))\n", k, x->n);
    } else {
        while (i >= 0 && x->key[i] > k) i--;
        i++;
        if (x->c[i]->n == BT_MAXK) {
            bt_split_child(x, i, verbose);
            if (x->key[i] < k) i++;
        }
        bt_insert_nonfull(x->c[i], k, verbose);
    }
}

static BNode *bt_insert(BNode *root, int k, int verbose)
{
    if (!root) { root = bt_new(1); root->key[0] = k; root->n = 1; return root; }
    if (root->n == BT_MAXK) {
        BNode *s = bt_new(0);
        s->c[0] = root;
        if (verbose) printf("    the ROOT is full -> the tree grows one level taller\n");
        bt_split_child(s, 0, verbose);
        bt_insert_nonfull(s, k, verbose);
        return s;
    }
    bt_insert_nonfull(root, k, verbose);
    return root;
}

static int bt_search(BNode *x, int k, int depth, int verbose)
{
    int i = 0, d;
    if (!x) return 0;
    if (verbose) {
        for (d = 0; d < depth; d++) printf("  ");
        printf("    scan node [");
        for (d = 0; d < x->n; d++) printf("%d%s", x->key[d], d < x->n - 1 ? "|" : "");
        printf("]\n");
    }
    while (i < x->n && k > x->key[i]) i++;
    if (i < x->n && k == x->key[i]) {
        if (verbose) { for (d = 0; d < depth; d++) printf("  ");
                       printf("    found %d at key position %d\n", k, i); }
        return 1;
    }
    if (x->leaf) {
        if (verbose) { for (d = 0; d < depth; d++) printf("  ");
                       printf("    leaf reached - %d is absent\n", k); }
        return 0;
    }
    if (verbose) { for (d = 0; d < depth; d++) printf("  ");
                   printf("    descend into child %d\n", i); }
    return bt_search(x->c[i], k, depth + 1, verbose);
}

static void bt_print_node(BNode *x)
{
    int i;
    printf("[");
    for (i = 0; i < x->n; i++) printf("%d%s", x->key[i], i < x->n - 1 ? "|" : "");
    printf("]");
}

static void bt_show(BNode *root)
{
    BNode *q[256];
    int head = 0, tail = 0, level = 0;
    if (!root) { printf("\n    ( B-Tree is empty )\n"); return; }
    printf("\n  B-TREE  (minimum degree t = %d, so %d..%d keys per node)\n",
           BT_T, BT_T - 1, BT_MAXK);
    q[tail++] = root;
    while (head < tail) {
        int cnt = tail - head, i, j;
        printf("    level %d :  ", level);
        for (i = 0; i < cnt; i++) {
            BNode *c = q[head + i];
            bt_print_node(c);
            printf("   ");
        }
        printf("\n");
        for (i = 0; i < cnt; i++) {
            BNode *c = q[head++];
            if (!c->leaf) for (j = 0; j <= c->n; j++) if (c->c[j]) q[tail++] = c->c[j];
        }
        level++;
    }
    printf("    (children of a node sit between its keys, in sorted order)\n");
}

static void bt_inorder(BNode *x, int out[], int *n)
{
    int i;
    if (!x) return;
    for (i = 0; i < x->n; i++) {
        if (!x->leaf) bt_inorder(x->c[i], out, n);
        out[(*n)++] = x->key[i];
    }
    if (!x->leaf) bt_inorder(x->c[x->n], out, n);
}

static void btree_menu(void)
{
    static const char *const items[] = {
        "Insert a key (splitting explained)", "Search a key",
        "Display the B-Tree", "Sorted (inorder) listing",
        "Demo : insert 10 20 5 6 12 30 7 17"
    };
    BNode *root = NULL;
    int choice, x, i;

    for (;;) {
        choice = ui_menu("B-TREE", items, 5, "Back (frees the tree)");
        if (choice == 0 || g_eof) { bt_free(root); return; }
        switch (choice) {
        case 1:
            x = ui_read_int("  Key to insert: ", -9999, 9999);
            printf("\n  BEFORE");
            bt_show(root);
            printf("\n  Inserting %d:\n", x);
            root = bt_insert(root, x, 1);
            printf("\n  AFTER");
            bt_show(root);
            break;
        case 2:
            if (!root) { printf("\n  Tree is empty.\n"); break; }
            x = ui_read_int("  Key to search: ", -9999, 9999);
            bt_show(root);
            printf("\n  Search path:\n");
            printf("\n  RESULT : %d is %s\n", x,
                   bt_search(root, x, 0, 1) ? "PRESENT" : "ABSENT");
            ui_complexity("B-Tree search", "O(1)", "O(log n)", "O(log n)",
                          "O(t * log_t n) nodes touched",
                          "Shallow tree - designed for disk blocks");
            break;
        case 3: bt_show(root); break;
        case 4: {
            int out[256], n = 0;
            bt_inorder(root, out, &n);
            printf("\n  Sorted keys : ");
            for (i = 0; i < n; i++) printf("%d ", out[i]);
            printf("\n  (%d keys)\n", n);
            break;
        }
        case 5: {
            int keys[8] = { 10, 20, 5, 6, 12, 30, 7, 17 };
            bt_free(root); root = NULL;
            for (i = 0; i < 8; i++) {
                printf("\n  ---- insert %d ----\n", keys[i]);
                root = bt_insert(root, keys[i], 1);
                bt_show(root);
                ui_step();
            }
            break;
        }
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ================================================================== menu */
void tree_menu(void)
{
    static const char *const items[] = {
        "Binary Search Tree (insert / delete / search / traversals)",
        "AVL Tree (rotations and the need for balance)",
        "B-Tree (multi-way search tree, node splitting)"
    };
    int choice;
    for (;;) {
        choice = ui_menu("TREES  (Unit 4)", items, 3, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: bst_menu();   break;
        case 2: avl_menu();   break;
        case 3: btree_menu(); break;
        default: break;
        }
    }
}
