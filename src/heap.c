/* =========================================================================
 *  heap.c  -  Unit-4 : Binary Heap (array implementation of a complete
 *             binary tree) and the heap based Priority Queue.
 *
 *             parent(i) = (i-1)/2     left(i) = 2i+1     right(i) = 2i+2
 * ========================================================================= */
#include "heap.h"
#include "viz.h"

static int  hp[MAX_ARRAY];
static int  hn    = 0;
static int  h_max = 1;                  /* 1 = max-heap, 0 = min-heap */

static int better(int a, int b) { return h_max ? a > b : a < b; }

static void hswap(int i, int j) { int t = hp[i]; hp[i] = hp[j]; hp[j] = t; }

static void heap_show(int e1, int e2, const char *cap)
{
    int i;
    if (cap) printf("\n  %s\n", cap);
    if (hn == 0) { printf("    ( heap is empty )\n"); return; }
    printf("  array form:\n");
    viz_array_plain(hp, hn);
    printf("  tree form (a complete binary tree, filled left to right):\n");
    viz_heap_tree(hp, hn, e1, e2);
    printf("\n  index relations  (parent (i-1)/2, children 2i+1 and 2i+2):\n");
    for (i = 0; i < hn && i < 7; i++) {
        char pb[20], lb[20], rb[20];
        if (i == 0) strcpy(pb, "   -");
        else sprintf(pb, "a[%d]=%d", (i - 1) / 2, hp[(i - 1) / 2]);
        if (2 * i + 1 < hn) sprintf(lb, "a[%d]=%d", 2 * i + 1, hp[2 * i + 1]);
        else strcpy(lb, "   -");
        if (2 * i + 2 < hn) sprintf(rb, "a[%d]=%d", 2 * i + 2, hp[2 * i + 2]);
        else strcpy(rb, "   -");
        printf("    i=%-2d value=%-5d parent=%-10s left=%-10s right=%-10s\n",
               i, hp[i], pb, lb, rb);
    }
    if (hn > 7) printf("    ... (%d more elements)\n", hn - 7);
}

/* ---- sift up ----------------------------------------------------------- */
static void sift_up(int i, int verbose)
{
    while (i > 0) {
        int p = (i - 1) / 2;
        if (verbose)
            printf("    compare child a[%d]=%d with parent a[%d]=%d\n",
                   i, hp[i], p, hp[p]);
        if (!better(hp[i], hp[p])) {
            if (verbose) printf("    heap property holds - stop\n");
            break;
        }
        if (verbose) printf("    out of order -> swap them\n");
        hswap(i, p);
        if (verbose) { heap_show(i, p, NULL); ui_step(); }
        i = p;
    }
}

/* ---- sift down --------------------------------------------------------- */
static void sift_down(int i, int n, int verbose)
{
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, best = i;
        if (l < n && better(hp[l], hp[best])) best = l;
        if (r < n && better(hp[r], hp[best])) best = r;
        if (verbose) {
            printf("    node a[%d]=%d, children:", i, hp[i]);
            if (l < n) printf(" a[%d]=%d", l, hp[l]);
            if (r < n) printf(" a[%d]=%d", r, hp[r]);
            printf("\n");
        }
        if (best == i) { if (verbose) printf("    heap property holds - stop\n"); break; }
        if (verbose) printf("    swap with a[%d]=%d\n", best, hp[best]);
        hswap(i, best);
        if (verbose) { heap_show(i, best, NULL); ui_step(); }
        i = best;
    }
}

/* ================================================================== menu */
void heap_menu(void)
{
    static const char *const items[] = {
        "Switch between MAX-heap and MIN-heap",
        "Insert a value (sift-up shown)",
        "Delete the root (sift-down shown)",
        "Build a heap from an array (bottom-up heapify)",
        "Peek root", "Display heap", "Clear heap",
        "Priority Queue using this heap"
    };
    int choice, x, i;

    for (;;) {
        char title[80];
        sprintf(title, "HEAP  (currently a %s-HEAP)", h_max ? "MAX" : "MIN");
        choice = ui_menu(title, items, 8, "Back to main menu");
        if (choice == 0 || g_eof) return;

        switch (choice) {
        case 1:
            h_max = !h_max;
            printf("\n  Now building a %s-heap. Rebuilding the existing data...\n",
                   h_max ? "MAX" : "MIN");
            for (i = hn / 2 - 1; i >= 0; i--) sift_down(i, hn, 0);
            heap_show(-1, -1, "REBUILT HEAP");
            break;
        case 2:
            if (hn == MAX_ARRAY) { printf("\n  !! Heap is full.\n"); break; }
            x = ui_read_int("  Value to insert: ", -9999, 9999);
            heap_show(-1, -1, "BEFORE");
            hp[hn] = x; hn++;
            printf("\n  Step 1 : place %d at the next free array slot (index %d)\n",
                   x, hn - 1);
            heap_show(hn - 1, -1, NULL);
            printf("\n  Step 2 : sift it UP until the heap property is restored\n");
            sift_up(hn - 1, 1);
            heap_show(-1, -1, "AFTER INSERT");
            ui_complexity("Heap insert", "O(1)", "O(log n)", "O(log n)",
                          "O(1) extra", "At most one swap per level");
            break;
        case 3:
            if (hn == 0) { printf("\n  !! Heap is empty.\n"); break; }
            heap_show(0, -1, "BEFORE");
            printf("\n  Step 1 : the root a[0]=%d is removed (that is the %s)\n",
                   hp[0], h_max ? "maximum" : "minimum");
            hp[0] = hp[hn - 1];
            hn--;
            printf("  Step 2 : move the LAST element into the root slot\n");
            heap_show(0, -1, NULL);
            printf("\n  Step 3 : sift it DOWN\n");
            sift_down(0, hn, 1);
            heap_show(-1, -1, "AFTER DELETE");
            ui_complexity("Heap delete root", "O(1)", "O(log n)", "O(log n)",
                          "O(1) extra", "Root always holds the extreme value");
            break;
        case 4: {
            int a[MAX_ARRAY], n;
            n = ui_build_array(a, 15, "the heap");
            for (i = 0; i < n; i++) hp[i] = a[i];
            hn = n;
            heap_show(-1, -1, "RAW ARRAY (not yet a heap)");
            printf("\n  Bottom-up build: heapify every internal node from "
                   "index %d down to 0.\n", hn / 2 - 1);
            printf("  Leaves (index %d .. %d) are already valid heaps of size 1.\n",
                   hn / 2, hn - 1);
            for (i = hn / 2 - 1; i >= 0; i--) {
                printf("\n  --- heapify sub-tree rooted at index %d (value %d) ---\n",
                       i, hp[i]);
                sift_down(i, hn, 1);
            }
            heap_show(0, -1, "FINISHED HEAP");
            ui_complexity("Build heap (bottom-up)", "O(n)", "O(n)", "O(n)",
                          "O(1) in place",
                          "Repeated insertion would instead cost O(n log n)");
            break;
        }
        case 5:
            if (hn == 0) printf("\n  Heap is empty.\n");
            else printf("\n  Root = %d  (the %s element, available in O(1))\n",
                        hp[0], h_max ? "largest" : "smallest");
            break;
        case 6: heap_show(-1, -1, "CURRENT HEAP"); break;
        case 7: hn = 0; printf("\n  Heap cleared.\n"); break;
        case 8: {
            int served = 0;
            if (hn == 0) { printf("\n  Build or insert some values first.\n"); break; }
            ui_header("PRIORITY QUEUE SERVED FROM THE HEAP");
            printf("\n  Dequeuing every element in priority order:\n\n  ");
            while (hn > 0) {
                printf("%d ", hp[0]);
                hp[0] = hp[hn - 1];
                hn--;
                sift_down(0, hn, 0);
                served++;
            }
            printf("\n\n  %d elements came out in %s order.\n", served,
                   h_max ? "descending" : "ascending");
            printf("  That is exactly Heap Sort: n deletions x O(log n) each "
                   "= O(n log n).\n");
            break;
        }
        default: break;
        }
        ui_pause(NULL);
    }
}
