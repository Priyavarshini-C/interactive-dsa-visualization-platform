/* =========================================================================
 *  queue.c  -  Unit-3 : Queue ADT
 *              Linear queue (array)      - shows the "false overflow" problem
 *              Circular queue (array)    - the fix, using modulo arithmetic
 *              Linked queue              - front and rear pointers
 *              Double ended queue (deque)
 *              Priority queue            - ordered insertion
 *              Application : CPU scheduling order
 * ========================================================================= */
#include "queue.h"
#include "viz.h"

#define QCAP 6

/* ------------------------------------------------------- shared drawing */
static void draw_slots(const int q[], int cap, const unsigned char *used,
                       int front, int rear)
{
    int i, j;
    printf("\n  array slots:\n   ");
    for (i = 0; i < cap; i++) printf("  %3d ", i);
    printf("   <- index\n  ");
    for (i = 0; i < cap; i++) {
        char f = used[i] ? '=' : '-';
        putchar('+');
        for (j = 0; j < 5; j++) putchar(f);
    }
    printf("+\n  ");
    for (i = 0; i < cap; i++) {
        if (used[i]) printf("|%4d ", q[i]);
        else         printf("|  .  ");
    }
    printf("|\n  ");
    for (i = 0; i < cap; i++) {
        char f = used[i] ? '=' : '-';
        putchar('+');
        for (j = 0; j < 5; j++) putchar(f);
    }
    printf("+\n  ");
    for (i = 0; i < cap; i++) {
        if (i == front && i == rear) printf("   FR ");
        else if (i == front)         printf("   F  ");
        else if (i == rear)          printf("   R  ");
        else                         printf("      ");
    }
    printf("\n    F = front index, R = rear index, '.' = empty slot\n");
}

/* ========================================================= LINEAR QUEUE */
static void linear_queue_menu(void)
{
    static const char *const items[] = {
        "Enqueue (insert at rear)", "Dequeue (delete from front)",
        "Peek front", "Display"
    };
    int q[QCAP], front = 0, rear = -1, choice, x, i;
    unsigned char used[QCAP];

    for (;;) {
        choice = ui_menu("LINEAR QUEUE (array implementation)", items, 4, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1:
            if (rear == QCAP - 1) {
                printf("\n  !! QUEUE OVERFLOW : rear has reached the last index.\n");
                if (front > 0)
                    printf("  Note: slots 0..%d are free but unusable in a LINEAR\n"
                           "  queue - this is the FALSE OVERFLOW problem that the\n"
                           "  CIRCULAR queue solves.\n", front - 1);
                break;
            }
            x = ui_read_int("  Value to enqueue: ", -9999, 9999);
            q[++rear] = x;
            printf("\n  rear = rear + 1;  queue[rear] = %d;\n", x);
            break;
        case 2:
            if (front > rear) { printf("\n  !! QUEUE UNDERFLOW : the queue is empty.\n");
                                break; }
            printf("\n  dequeued %d;  front = front + 1;\n", q[front]);
            front++;
            break;
        case 3:
            if (front > rear) printf("\n  Queue is empty.\n");
            else printf("\n  FRONT element = %d\n", q[front]);
            break;
        default: break;
        }
        for (i = 0; i < QCAP; i++)
            used[i] = (unsigned char)(i >= front && i <= rear);
        draw_slots(q, QCAP, used, front <= rear ? front : -1,
                   front <= rear ? rear : -1);
        printf("    size = %d\n", front <= rear ? rear - front + 1 : 0);
        ui_complexity("Queue (array)", "O(1)", "O(1)", "O(1)",
                      "O(n) slots",
                      "FIFO - first in, first out");
        ui_pause(NULL);
    }
}

/* ======================================================= CIRCULAR QUEUE */
static void circular_queue_menu(void)
{
    static const char *const items[] = {
        "Enqueue", "Dequeue", "Peek front", "Display"
    };
    int q[QCAP], front = 0, rear = -1, count = 0, choice, x, i;
    unsigned char used[QCAP];

    for (;;) {
        choice = ui_menu("CIRCULAR QUEUE (array + modulo arithmetic)",
                         items, 4, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1:
            if (count == QCAP) {
                printf("\n  !! QUEUE FULL : all %d slots are occupied "
                       "(a real overflow, not a false one).\n", QCAP);
                break;
            }
            x = ui_read_int("  Value to enqueue: ", -9999, 9999);
            rear = (rear + 1) % QCAP;
            q[rear] = x;
            count++;
            printf("\n  rear = (rear + 1) %% %d = %d;  queue[%d] = %d;\n",
                   QCAP, rear, rear, x);
            if (rear < front) printf("  the rear has WRAPPED AROUND past the end "
                                     "of the array.\n");
            break;
        case 2:
            if (count == 0) { printf("\n  !! QUEUE EMPTY : nothing to dequeue.\n");
                              break; }
            printf("\n  dequeued %d;  front = (front + 1) %% %d = %d;\n",
                   q[front], QCAP, (front + 1) % QCAP);
            front = (front + 1) % QCAP;
            count--;
            break;
        case 3:
            if (count == 0) printf("\n  Queue is empty.\n");
            else printf("\n  FRONT element = %d\n", q[front]);
            break;
        default: break;
        }
        for (i = 0; i < QCAP; i++) used[i] = 0;
        for (i = 0; i < count; i++) used[(front + i) % QCAP] = 1;
        draw_slots(q, QCAP, used, count ? front : -1, count ? rear : -1);
        printf("    count = %d of %d\n", count, QCAP);
        printf("\n  logical order (front to rear): ");
        if (!count) printf("( empty )");
        for (i = 0; i < count; i++) printf("%d ", q[(front + i) % QCAP]);
        printf("\n");
        ui_pause(NULL);
    }
}

/* ========================================================= LINKED QUEUE */
typedef struct QNode { int data; struct QNode *next; } QNode;

static void linked_queue_menu(void)
{
    static const char *const items[] = { "Enqueue", "Dequeue", "Display" };
    QNode *front = NULL, *rear = NULL, *p;
    int choice, x, v[MAX_ARRAY], n;

    for (;;) {
        choice = ui_menu("LINKED QUEUE (front and rear pointers)", items, 3,
                         "Back (frees all nodes)");
        if (choice == 0 || g_eof) {
            while (front) { p = front->next; free(front); front = p; }
            return;
        }
        switch (choice) {
        case 1:
            x = ui_read_int("  Value to enqueue: ", -9999, 9999);
            p = (QNode *)xmalloc(sizeof(QNode));
            p->data = x; p->next = NULL;
            if (!rear) front = rear = p;
            else { rear->next = p; rear = p; }
            printf("\n  node appended at the REAR - O(1) because we keep a rear pointer.\n");
            break;
        case 2:
            if (!front) { printf("\n  !! QUEUE UNDERFLOW.\n"); break; }
            p = front; x = p->data; front = front->next;
            if (!front) rear = NULL;
            free(p);
            printf("\n  dequeued %d from the FRONT, node freed.\n", x);
            break;
        default: break;
        }
        n = 0;
        for (p = front; p && n < MAX_ARRAY; p = p->next) v[n++] = p->data;
        printf("\n  FRONT ... REAR\n");
        viz_chain(v, n, 0, -1, "FRONT");
        printf("    nodes = %d   (rear points at the last node)\n", n);
        ui_pause(NULL);
    }
}

/* ================================================================ DEQUE */
static void deque_menu(void)
{
    static const char *const items[] = {
        "Insert at FRONT", "Insert at REAR",
        "Delete from FRONT", "Delete from REAR", "Display"
    };
    int dq[QCAP], front = 0, rear = -1, count = 0, choice, x, i;
    unsigned char used[QCAP];

    for (;;) {
        choice = ui_menu("DOUBLE ENDED QUEUE (deque)", items, 5, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1:
            if (count == QCAP) { printf("\n  !! DEQUE FULL.\n"); break; }
            x = ui_read_int("  Value: ", -9999, 9999);
            front = (front - 1 + QCAP) % QCAP;
            if (count == 0) rear = front;
            dq[front] = x; count++;
            printf("\n  front = (front - 1 + %d) %% %d = %d\n", QCAP, QCAP, front);
            break;
        case 2:
            if (count == QCAP) { printf("\n  !! DEQUE FULL.\n"); break; }
            x = ui_read_int("  Value: ", -9999, 9999);
            rear = (rear + 1) % QCAP;
            if (count == 0) front = rear;
            dq[rear] = x; count++;
            printf("\n  rear = (rear + 1) %% %d = %d\n", QCAP, rear);
            break;
        case 3:
            if (!count) { printf("\n  !! DEQUE EMPTY.\n"); break; }
            printf("\n  removed %d from the front.\n", dq[front]);
            front = (front + 1) % QCAP; count--;
            if (!count) { front = 0; rear = -1; }
            break;
        case 4:
            if (!count) { printf("\n  !! DEQUE EMPTY.\n"); break; }
            printf("\n  removed %d from the rear.\n", dq[rear]);
            rear = (rear - 1 + QCAP) % QCAP; count--;
            if (!count) { front = 0; rear = -1; }
            break;
        default: break;
        }
        for (i = 0; i < QCAP; i++) used[i] = 0;
        for (i = 0; i < count; i++) used[(front + i) % QCAP] = 1;
        draw_slots(dq, QCAP, used, count ? front : -1, count ? rear : -1);
        printf("\n  logical order: ");
        if (!count) printf("( empty )");
        for (i = 0; i < count; i++) printf("%d ", dq[(front + i) % QCAP]);
        printf("\n  Insertion and deletion are allowed at BOTH ends.\n");
        ui_pause(NULL);
    }
}

/* ======================================================= PRIORITY QUEUE */
typedef struct { int value, prio; } PItem;

static void pq_show(PItem h[], int n)
{
    int i;
    printf("\n  PRIORITY QUEUE (kept ordered; smaller number = higher priority)\n");
    printf("  +------+----------+------------+\n");
    printf("  | slot |  value   |  priority  |\n");
    printf("  +------+----------+------------+\n");
    for (i = 0; i < n; i++)
        printf("  | %4d | %8d | %10d |%s\n", i, h[i].value, h[i].prio,
               i == 0 ? "  <- next to be served" : "");
    if (!n) printf("  |            ( empty )             |\n");
    printf("  +------+----------+------------+\n");
}

static void priority_queue_menu(void)
{
    static const char *const items[] = {
        "Enqueue with a priority", "Dequeue highest priority", "Display",
        "Application : run a CPU scheduling order"
    };
    PItem h[MAX_ARRAY];
    int n = 0, choice, i, j;

    for (;;) {
        choice = ui_menu("PRIORITY QUEUE (ordered insertion)", items, 4, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: {
            PItem it;
            if (n == MAX_ARRAY) { printf("\n  !! FULL.\n"); break; }
            it.value = ui_read_int("  Value (e.g. a process id): ", 0, 9999);
            it.prio  = ui_read_int("  Priority (1 = most urgent): ", 1, 99);
            for (i = n; i > 0 && h[i - 1].prio > it.prio; i--) h[i] = h[i - 1];
            h[i] = it; n++;
            printf("\n  inserted at slot %d so the queue stays ordered - O(n).\n", i);
            pq_show(h, n);
            break;
        }
        case 2:
            if (!n) { printf("\n  !! EMPTY.\n"); break; }
            printf("\n  served value %d (priority %d)\n", h[0].value, h[0].prio);
            for (i = 0; i < n - 1; i++) h[i] = h[i + 1];
            n--;
            pq_show(h, n);
            break;
        case 3: pq_show(h, n); break;
        case 4: {
            int pid[5]  = { 101, 102, 103, 104, 105 };
            int pri[5]  = {   3,   1,   4,   1,   2 };
            int burst[5]= {   5,   3,   2,   4,   6 };
            int order[5], used[5] = {0,0,0,0,0}, t = 0;
            ui_header("APPLICATION : PRIORITY SCHEDULING");
            printf("\n  +-------+----------+-------+\n");
            printf("  |  PID  | priority | burst |\n");
            printf("  +-------+----------+-------+\n");
            for (i = 0; i < 5; i++)
                printf("  | %5d | %8d | %5d |\n", pid[i], pri[i], burst[i]);
            printf("  +-------+----------+-------+\n");
            for (i = 0; i < 5; i++) {
                int best = -1;
                for (j = 0; j < 5; j++)
                    if (!used[j] && (best == -1 || pri[j] < pri[best])) best = j;
                used[best] = 1; order[i] = best;
            }
            printf("\n  Execution order chosen by the priority queue:\n\n  ");
            for (i = 0; i < 5; i++) {
                printf("[P%d]", pid[order[i]]);
                if (i < 4) printf(" -> ");
            }
            printf("\n\n  Gantt chart:\n  ");
            for (i = 0; i < 5; i++) { printf("+"); for (j = 0; j < burst[order[i]] * 2; j++) putchar('-'); }
            printf("+\n  ");
            for (i = 0; i < 5; i++) {
                printf("|");
                for (j = 0; j < burst[order[i]] * 2; j++) putchar(' ');
            }
            printf("|\n  ");
            for (i = 0; i < 5; i++) { printf("+"); for (j = 0; j < burst[order[i]] * 2; j++) putchar('-'); }
            printf("+\n  ");
            printf("%-2d", t);
            for (i = 0; i < 5; i++) { t += burst[order[i]];
                for (j = 0; j < burst[order[i]] * 2 - 1; j++) putchar(' ');
                printf("%-2d", t); }
            printf("\n");
            break;
        }
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ================================================================== menu */
void queue_menu(void)
{
    static const char *const items[] = {
        "Linear Queue (array)",
        "Circular Queue (array)",
        "Linked Queue",
        "Double Ended Queue (deque)",
        "Priority Queue + scheduling application"
    };
    int choice;
    for (;;) {
        choice = ui_menu("QUEUE  (Unit 3)", items, 5, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: linear_queue_menu();   break;
        case 2: circular_queue_menu(); break;
        case 3: linked_queue_menu();   break;
        case 4: deque_menu();          break;
        case 5: priority_queue_menu(); break;
        default: break;
        }
    }
}
