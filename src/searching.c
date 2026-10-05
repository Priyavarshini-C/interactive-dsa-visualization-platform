/* =========================================================================
 *  searching.c  -  Linear Search and Binary Search with live visualisation
 *  Syllabus: 21CSC201J CLR-1 / CO-2 (searching techniques + complexity)
 * ========================================================================= */
#include "searching.h"
#include "viz.h"

/* ------------------------------------------------------------------------ */
int linear_search(const int a[], int n, int key, Stats *st, int verbose)
{
    int i;
    for (i = 0; i < n; i++) {
        st->comparisons++;
        if (verbose) {
            printf("\n  Step %d : compare a[%d]=%d with key=%d\n",
                   i + 1, i, a[i], key);
            viz_array_1(a, n, i, "i");
            if (a[i] == key) printf("    %d == %d  -> MATCH FOUND\n", a[i], key);
            else             printf("    %d != %d  -> move right\n", a[i], key);
            ui_step();
        }
        if (a[i] == key) return i;
    }
    return -1;
}

/* ------------------------------------------------------------------------ */
int binary_search(const int a[], int n, int key, Stats *st, int verbose)
{
    int low = 0, high = n - 1, mid, step = 0;
    unsigned char emph[MAX_ARRAY];
    Marker mk[3];

    while (low <= high) {
        int i;
        mid = low + (high - low) / 2;        /* overflow-safe midpoint */
        step++;
        if (verbose) {
            for (i = 0; i < n; i++)
                emph[i] = (unsigned char)(i >= low && i <= high);
            mk[0].idx = low;  mk[0].label = "LOW";
            mk[1].idx = mid;  mk[1].label = "MID";
            mk[2].idx = high; mk[2].label = "HIGH";
            printf("\n  Step %d : search window a[%d..%d], mid=%d (value %d)\n",
                   step, low, high, mid, a[mid]);
            printf("    (cells with '=' borders are still under consideration)\n");
            viz_array(a, n, emph, mk, 3);
        }
        st->comparisons++;
        if (a[mid] == key) {
            if (verbose) {
                printf("    a[mid]=%d == key=%d  -> FOUND at index %d\n",
                       a[mid], key, mid);
                ui_step();
            }
            return mid;
        }
        st->comparisons++;
        if (a[mid] < key) {
            if (verbose)
                printf("    a[mid]=%d < key=%d  -> discard left half, "
                       "low = mid+1 = %d\n", a[mid], key, mid + 1);
            low = mid + 1;
        } else {
            if (verbose)
                printf("    a[mid]=%d > key=%d  -> discard right half, "
                       "high = mid-1 = %d\n", a[mid], key, mid - 1);
            high = mid - 1;
        }
        if (verbose) ui_step();
    }
    return -1;
}

/* ------------------------------------------------------------- local sort */
static void ensure_sorted(int a[], int n)
{
    int i, j, key;
    for (i = 1; i < n; i++) {           /* insertion sort - a utility here,  */
        key = a[i];                     /* not the algorithm being taught    */
        for (j = i - 1; j >= 0 && a[j] > key; j--) a[j + 1] = a[j];
        a[j + 1] = key;
    }
}

static int is_sorted(const int a[], int n)
{
    int i;
    for (i = 1; i < n; i++) if (a[i - 1] > a[i]) return 0;
    return 1;
}

/* ------------------------------------------------------------------- menu */
static void run_search(int which)
{
    int a[MAX_ARRAY], n, key, pos;
    Stats st;

    stats_reset(&st);
    n = ui_build_array(a, MAX_ARRAY, which == 1 ? "Linear Search"
                                                : "Binary Search");
    if (which == 2 && !is_sorted(a, n)) {
        ui_header("PRE-CONDITION CHECK");
        ui_note("Binary Search requires a sorted array.");
        ui_note("The data you supplied is unsorted, so it is sorted first.");
        printf("\n  Before sorting:\n");
        viz_array_plain(a, n);
        ensure_sorted(a, n);
        printf("\n  After sorting:\n");
        viz_array_plain(a, n);
        ui_step();
    }

    ui_header(which == 1 ? "LINEAR SEARCH" : "BINARY SEARCH");
    printf("\n  Array under search:\n");
    viz_array_plain(a, n);
    key = ui_read_int("\n  Key to search for: ", -9999, 9999);

    pos = (which == 1) ? linear_search(a, n, key, &st, 1)
                       : binary_search(a, n, key, &st, 1);

    ui_header("RESULT");
    if (pos >= 0) {
        printf("  Key %d FOUND at index %d (position %d).\n", key, pos, pos + 1);
        viz_array_1(a, n, pos, "FOUND");
    } else {
        printf("  Key %d is NOT PRESENT in the array.\n", key);
        viz_array_plain(a, n);
    }
    printf("\n  Comparisons performed : %ld\n", st.comparisons);

    if (which == 1)
        ui_complexity("Linear Search", "O(1)  - key is the first element",
                      "O(n)", "O(n)  - key last or absent",
                      "O(1)  - iterative, no extra storage",
                      "Works on unsorted data");
    else
        ui_complexity("Binary Search", "O(1)  - key is the middle element",
                      "O(log n)", "O(log n)",
                      "O(1) iterative / O(log n) recursive stack",
                      "Requires the array to be sorted");
    ui_pause("  [Enter] to return to the Searching menu ");
}

void searching_menu(void)
{
    static const char *const items[] = {
        "Linear Search  (sequential scan)",
        "Binary Search  (divide and conquer, sorted data)",
        "Compare Linear vs Binary on the same data"
    };
    int choice;

    for (;;) {
        choice = ui_menu("SEARCHING ALGORITHMS", items, 3, "Back to main menu");
        if (choice == 0 || g_eof) return;
        if (choice == 3) {
            int a[MAX_ARRAY], n, key;
            Stats sl, sb;
            int pl, pb;
            stats_reset(&sl); stats_reset(&sb);
            n = ui_build_array(a, MAX_ARRAY, "the comparison run");
            ensure_sorted(a, n);
            ui_header("LINEAR vs BINARY  (same sorted data, same key)");
            viz_array_plain(a, n);
            key = ui_read_int("\n  Key to search for: ", -9999, 9999);
            pl = linear_search(a, n, key, &sl, 0);
            pb = binary_search(a, n, key, &sb, 0);
            printf("\n  +----------------------+--------------+---------------+\n");
            printf("  | Algorithm            | Comparisons  | Result index  |\n");
            printf("  +----------------------+--------------+---------------+\n");
            printf("  | Linear Search        | %12ld | %13d |\n", sl.comparisons, pl);
            printf("  | Binary Search        | %12ld | %13d |\n", sb.comparisons, pb);
            printf("  +----------------------+--------------+---------------+\n");
            printf("\n  (index -1 means the key is absent)\n");
            ui_note("Linear needs O(n) comparisons; binary needs O(log n).");
            ui_pause(NULL);
        } else {
            run_search(choice);
        }
    }
}
