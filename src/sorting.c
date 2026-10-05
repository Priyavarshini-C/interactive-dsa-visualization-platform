/* =========================================================================
 *  sorting.c  -  Bubble / Selection / Insertion / Merge / Quick / Heap sort
 *  Every algorithm is written out by hand - no library sort is used.
 *  Syllabus: 21CSC201J CLR-1 / CO-2 (sorting techniques + complexity)
 * ========================================================================= */
#include "sorting.h"
#include "viz.h"

static void swap_i(int *x, int *y) { int t = *x; *x = *y; *y = t; }

/* ====================================================== 1. BUBBLE SORT */
void bubble_sort(int a[], int n, Stats *st, int verbose)
{
    int i, j, swapped;
    unsigned char emph[MAX_ARRAY];

    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        if (verbose) printf("\n  ---- PASS %d ----\n", i + 1);
        for (j = 0; j < n - 1 - i; j++) {
            st->comparisons++;
            if (verbose) {
                printf("\n  compare a[%d]=%d with a[%d]=%d\n",
                       j, a[j], j + 1, a[j + 1]);
                viz_array_2(a, n, j, "j", j + 1, "j+1");
            }
            if (a[j] > a[j + 1]) {
                swap_i(&a[j], &a[j + 1]);
                st->swaps++;
                swapped = 1;
                if (verbose) {
                    printf("    %d > %d  -> SWAP\n", a[j + 1], a[j]);
                    viz_array_2(a, n, j, "j", j + 1, "j+1");
                }
            } else if (verbose) {
                printf("    %d <= %d -> no swap\n", a[j], a[j + 1]);
            }
            if (verbose) ui_step();
        }
        if (verbose) {
            int k;
            for (k = 0; k < n; k++) emph[k] = (unsigned char)(k >= n - 1 - i);
            printf("\n  End of pass %d - largest %d element(s) now in place:\n",
                   i + 1, i + 1);
            viz_array(a, n, emph, NULL, 0);
            viz_bars(a, n, emph);
        }
        if (!swapped) {
            if (verbose)
                printf("\n  No swaps in this pass -> array already sorted, "
                       "stop early.\n");
            break;
        }
    }
}

/* =================================================== 2. SELECTION SORT */
void selection_sort(int a[], int n, Stats *st, int verbose)
{
    int i, j, min;
    unsigned char emph[MAX_ARRAY];

    for (i = 0; i < n - 1; i++) {
        min = i;
        if (verbose)
            printf("\n  ---- PASS %d : find the smallest value in a[%d..%d] ----\n",
                   i + 1, i, n - 1);
        for (j = i + 1; j < n; j++) {
            st->comparisons++;
            if (verbose) {
                printf("\n  compare a[%d]=%d with current minimum a[%d]=%d\n",
                       j, a[j], min, a[min]);
                viz_array_3(a, n, i, "i", min, "min", j, "j");
            }
            if (a[j] < a[min]) {
                min = j;
                if (verbose) printf("    new minimum -> index %d (%d)\n", min, a[min]);
            }
            if (verbose) ui_step();
        }
        if (min != i) {
            swap_i(&a[i], &a[min]);
            st->swaps++;
            if (verbose) printf("\n  swap a[%d] with a[%d]\n", i, min);
        } else if (verbose) {
            printf("\n  a[%d] is already the minimum - no swap\n", i);
        }
        if (verbose) {
            int k;
            for (k = 0; k < n; k++) emph[k] = (unsigned char)(k <= i);
            viz_array(a, n, emph, NULL, 0);
            printf("  sorted prefix a[0..%d]\n", i);
            ui_step();
        }
    }
}

/* =================================================== 3. INSERTION SORT */
void insertion_sort(int a[], int n, Stats *st, int verbose)
{
    int i, j, key;
    unsigned char emph[MAX_ARRAY];

    for (i = 1; i < n; i++) {
        key = a[i];
        j   = i - 1;
        if (verbose) {
            int k;
            for (k = 0; k < n; k++) emph[k] = (unsigned char)(k < i);
            printf("\n  ---- PASS %d : insert key = a[%d] = %d into the "
                   "sorted prefix ----\n", i, i, key);
            viz_array_1(a, n, i, "key");
            viz_array(a, n, emph, NULL, 0);
            printf("  (cells with '=' borders are the already-sorted prefix)\n");
        }
        while (j >= 0) {
            st->comparisons++;
            if (a[j] <= key) break;
            if (verbose)
                printf("    a[%d]=%d > key=%d  -> shift it one place right\n",
                       j, a[j], key);
            a[j + 1] = a[j];
            st->moves++;
            j--;
            if (verbose) { viz_array_1(a, n, j + 1, "hole"); ui_step(); }
        }
        a[j + 1] = key;
        st->moves++;
        if (verbose) {
            printf("    place key %d at index %d\n", key, j + 1);
            viz_array_1(a, n, j + 1, "key");
            ui_step();
        }
    }
}

/* ======================================================= 4. MERGE SORT */
static void merge_parts(int a[], int lo, int mid, int hi, Stats *st,
                        int verbose, int n)
{
    int  nl = mid - lo + 1, nr = hi - mid;
    int *L  = (int *)xmalloc(sizeof(int) * (size_t)(nl > 0 ? nl : 1));
    int *R  = (int *)xmalloc(sizeof(int) * (size_t)(nr > 0 ? nr : 1));
    int  i, j, k;
    unsigned char emph[MAX_ARRAY];

    for (i = 0; i < nl; i++) L[i] = a[lo + i];
    for (j = 0; j < nr; j++) R[j] = a[mid + 1 + j];

    if (verbose) {
        printf("\n  MERGE a[%d..%d] with a[%d..%d]\n", lo, mid, mid + 1, hi);
        printf("    left  : "); for (i = 0; i < nl; i++) printf("%d ", L[i]);
        printf("\n    right : "); for (j = 0; j < nr; j++) printf("%d ", R[j]);
        printf("\n");
    }

    i = j = 0; k = lo;
    while (i < nl && j < nr) {
        st->comparisons++;
        if (L[i] <= R[j]) {
            if (verbose) printf("      %d <= %d  -> take %d from LEFT\n",
                                L[i], R[j], L[i]);
            a[k++] = L[i++];
        } else {
            if (verbose) printf("      %d >  %d  -> take %d from RIGHT\n",
                                L[i], R[j], R[j]);
            a[k++] = R[j++];
        }
        st->moves++;
    }
    while (i < nl) { a[k++] = L[i++]; st->moves++; }
    while (j < nr) { a[k++] = R[j++]; st->moves++; }

    if (verbose) {
        int t;
        for (t = 0; t < n; t++) emph[t] = (unsigned char)(t >= lo && t <= hi);
        printf("    merged segment a[%d..%d]:\n", lo, hi);
        viz_array(a, n, emph, NULL, 0);
        ui_step();
    }
    free(L); free(R);
}

static void merge_rec(int a[], int lo, int hi, Stats *st, int verbose, int n,
                      int depth)
{
    int mid, d;
    if (lo >= hi) return;
    mid = lo + (hi - lo) / 2;
    if (verbose) {
        printf("\n  ");
        for (d = 0; d < depth; d++) printf("| ");
        printf("DIVIDE a[%d..%d] -> a[%d..%d] and a[%d..%d]\n",
               lo, hi, lo, mid, mid + 1, hi);
    }
    merge_rec(a, lo, mid, st, verbose, n, depth + 1);
    merge_rec(a, mid + 1, hi, st, verbose, n, depth + 1);
    merge_parts(a, lo, mid, hi, st, verbose, n);
}

void merge_sort(int a[], int n, Stats *st, int verbose)
{
    if (n > 1) merge_rec(a, 0, n - 1, st, verbose, n, 0);
}

/* ======================================================= 5. QUICK SORT */
static int partition_lomuto(int a[], int lo, int hi, Stats *st, int verbose,
                            int n)
{
    int pivot = a[hi], i = lo - 1, j;

    if (verbose) {
        printf("\n  PARTITION a[%d..%d], pivot = a[%d] = %d\n", lo, hi, hi, pivot);
        viz_array_1(a, n, hi, "pivot");
    }
    for (j = lo; j < hi; j++) {
        st->comparisons++;
        if (verbose) printf("    compare a[%d]=%d with pivot %d\n", j, a[j], pivot);
        if (a[j] <= pivot) {
            i++;
            if (i != j) {
                swap_i(&a[i], &a[j]);
                st->swaps++;
                if (verbose) printf("      %d <= %d -> swap a[%d] and a[%d]\n",
                                    a[j], pivot, i, j);
            } else if (verbose) {
                printf("      %d <= %d -> already in the left part\n",
                       a[i], pivot);
            }
            if (verbose) viz_array_3(a, n, i, "i", j, "j", hi, "pivot");
        } else if (verbose) {
            printf("      %d >  %d -> stays in the right part\n", a[j], pivot);
        }
        if (verbose) ui_step();
    }
    swap_i(&a[i + 1], &a[hi]);
    st->swaps++;
    if (verbose) {
        printf("    place pivot at its final index %d\n", i + 1);
        viz_array_1(a, n, i + 1, "pivot fixed");
        ui_step();
    }
    return i + 1;
}

static void quick_rec(int a[], int lo, int hi, Stats *st, int verbose, int n)
{
    int p;
    if (lo >= hi) return;
    p = partition_lomuto(a, lo, hi, st, verbose, n);
    quick_rec(a, lo, p - 1, st, verbose, n);
    quick_rec(a, p + 1, hi, st, verbose, n);
}

void quick_sort(int a[], int n, Stats *st, int verbose)
{
    if (n > 1) quick_rec(a, 0, n - 1, st, verbose, n);
}

/* ======================================================== 6. HEAP SORT */
static void sift_down(int a[], int n, int i, Stats *st, int verbose,
                      int total)
{
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, big = i;
        if (l < n) { st->comparisons++; if (a[l] > a[big]) big = l; }
        if (r < n) { st->comparisons++; if (a[r] > a[big]) big = r; }
        if (big == i) break;
        if (verbose)
            printf("    a[%d]=%d is smaller than child a[%d]=%d -> swap down\n",
                   i, a[i], big, a[big]);
        swap_i(&a[i], &a[big]);
        st->swaps++;
        if (verbose) { viz_heap_tree(a, total, i, big); ui_step(); }
        i = big;
    }
}

void heap_sort(int a[], int n, Stats *st, int verbose)
{
    int i;
    if (verbose) {
        printf("\n  PHASE 1 : build a MAX-HEAP from the array\n");
        printf("  array viewed as a complete binary tree:\n");
        viz_heap_tree(a, n, -1, -1);
        ui_step();
    }
    for (i = n / 2 - 1; i >= 0; i--) {
        if (verbose) printf("\n  heapify sub-tree rooted at index %d (value %d)\n",
                            i, a[i]);
        sift_down(a, n, i, st, verbose, n);
    }
    if (verbose) {
        printf("\n  MAX-HEAP ready (root %d is the largest element):\n", a[0]);
        viz_heap_tree(a, n, 0, -1);
        viz_array_plain(a, n);
        ui_step();
        printf("\n  PHASE 2 : repeatedly move the root to the end\n");
    }
    for (i = n - 1; i > 0; i--) {
        if (verbose)
            printf("\n  swap root a[0]=%d with a[%d]=%d, heap size becomes %d\n",
                   a[0], i, a[i], i);
        swap_i(&a[0], &a[i]);
        st->swaps++;
        if (verbose) {
            unsigned char emph[MAX_ARRAY];
            int k;
            for (k = 0; k < n; k++) emph[k] = (unsigned char)(k >= i);
            viz_array(a, n, emph, NULL, 0);
            printf("  (cells with '=' borders are sorted and out of the heap)\n");
            viz_heap_tree(a, i, 0, -1);
        }
        sift_down(a, i, 0, st, verbose, i);
        if (verbose) ui_step();
    }
}

/* ======================================================= dispatch tables */
const char *sort_name(int id)
{
    switch (id) {
    case 1: return "Bubble Sort";
    case 2: return "Selection Sort";
    case 3: return "Insertion Sort";
    case 4: return "Merge Sort";
    case 5: return "Quick Sort";
    case 6: return "Heap Sort";
    default: return "Unknown";
    }
}

void sort_run(int id, int a[], int n, Stats *st, int verbose)
{
    switch (id) {
    case 1: bubble_sort   (a, n, st, verbose); break;
    case 2: selection_sort(a, n, st, verbose); break;
    case 3: insertion_sort(a, n, st, verbose); break;
    case 4: merge_sort    (a, n, st, verbose); break;
    case 5: quick_sort    (a, n, st, verbose); break;
    case 6: heap_sort     (a, n, st, verbose); break;
    default: break;
    }
}

void sort_complexity(int id)
{
    switch (id) {
    case 1: ui_complexity("Bubble Sort", "O(n) - already sorted (early exit)",
                          "O(n^2)", "O(n^2)", "O(1) - in place",
                          "Stable"); break;
    case 2: ui_complexity("Selection Sort", "O(n^2)", "O(n^2)", "O(n^2)",
                          "O(1) - in place",
                          "Not stable; always n-1 swaps at most"); break;
    case 3: ui_complexity("Insertion Sort", "O(n) - already sorted",
                          "O(n^2)", "O(n^2) - reverse sorted",
                          "O(1) - in place",
                          "Stable; good for nearly sorted data"); break;
    case 4: ui_complexity("Merge Sort", "O(n log n)", "O(n log n)",
                          "O(n log n)",
                          "O(n) auxiliary array + O(log n) stack",
                          "Stable; divide and conquer"); break;
    case 5: ui_complexity("Quick Sort", "O(n log n)", "O(n log n)",
                          "O(n^2) - already sorted with last-element pivot",
                          "O(log n) average recursion stack",
                          "Not stable; in place"); break;
    case 6: ui_complexity("Heap Sort", "O(n log n)", "O(n log n)",
                          "O(n log n)", "O(1) - in place",
                          "Not stable; build-heap is O(n)"); break;
    default: break;
    }
}

/* ================================================================= menu */
void sorting_menu(void)
{
    static const char *const items[] = {
        "Bubble Sort",
        "Selection Sort",
        "Insertion Sort",
        "Merge Sort      (divide and conquer)",
        "Quick Sort      (partitioning)",
        "Heap Sort       (array as a binary heap)"
    };
    int choice, n, i;
    int a[MAX_ARRAY], orig[MAX_ARRAY];
    Stats st;

    for (;;) {
        choice = ui_menu("SORTING ALGORITHMS", items, 6, "Back to main menu");
        if (choice == 0 || g_eof) return;

        n = ui_build_array(a, 15, sort_name(choice));
        for (i = 0; i < n; i++) orig[i] = a[i];
        stats_reset(&st);

        ui_header(sort_name(choice));
        printf("\n  INITIAL ARRAY\n");
        viz_array_plain(a, n);
        viz_bars(a, n, NULL);
        ui_step();

        sort_run(choice, a, n, &st, 1);

        ui_header("FINAL RESULT");
        printf("\n  Before : ");
        for (i = 0; i < n; i++) printf("%d ", orig[i]);
        printf("\n  After  : ");
        for (i = 0; i < n; i++) printf("%d ", a[i]);
        printf("\n\n");
        viz_array_plain(a, n);
        viz_bars(a, n, NULL);
        printf("\n  Comparisons : %ld\n", st.comparisons);
        printf("  Swaps       : %ld\n", st.swaps);
        if (st.moves) printf("  Element moves/shifts : %ld\n", st.moves);
        {   /* verification against the definition of "sorted" */
            int ok = 1;
            for (i = 1; i < n; i++) if (a[i - 1] > a[i]) ok = 0;
            printf("  Verification: array is %s\n",
                   ok ? "correctly sorted (non-decreasing)" : "NOT SORTED!");
        }
        sort_complexity(choice);
        ui_pause("  [Enter] to return to the Sorting menu ");
    }
}
