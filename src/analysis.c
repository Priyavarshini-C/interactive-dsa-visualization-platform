/* =========================================================================
 *  analysis.c  -  Unit-1 : asymptotic notation, complexity, time-space
 *                 trade off, plus a comparison lab whose numbers are
 *                 MEASURED by actually running the implementations.
 * ========================================================================= */
#include "analysis.h"
#include "sorting.h"
#include "searching.h"
#include "viz.h"

/* ------------------------------------------------------ growth rate table */
static int ilog2(int n) { int r = 0; while (n > 1) { n /= 2; r++; } return r; }

static void growth_table(void)
{
    int sizes[6] = { 8, 16, 32, 64, 128, 1024 };
    int i;
    ui_header("HOW THE COMMON GROWTH RATES BEHAVE");
    printf("\n  +--------+--------+--------+-----------+-------------+\n");
    printf("  |    n   | log2 n |    n   |  n log2 n |     n^2     |\n");
    printf("  +--------+--------+--------+-----------+-------------+\n");
    for (i = 0; i < 6; i++) {
        int n = sizes[i];
        printf("  | %6d | %6d | %6d | %9d | %11ld |\n",
               n, ilog2(n), n, n * ilog2(n), (long)n * n);
    }
    printf("  +--------+--------+--------+-----------+-------------+\n");
    printf("\n  For n = 1024 an O(n^2) algorithm does about %ld steps while an\n",
           (long)1024 * 1024);
    printf("  O(n log n) algorithm does about %d - roughly %ld times fewer.\n",
           1024 * ilog2(1024), (long)1024 * 1024 / (1024 * ilog2(1024)));
    printf("\n  Visual shape (relative cost at n = 8, 16, 32, 64):\n");
    for (i = 0; i < 4; i++) {
        int n = sizes[i], k;
        printf("    n=%-4d  log n |", n);
        for (k = 0; k < ilog2(n); k++) putchar('#');
        printf("\n            n     |");
        for (k = 0; k < n / 2; k++) putchar('#');
        printf("\n            n^2   |");
        for (k = 0; k < n * n / 64 && k < 64; k++) putchar('#');
        printf("%s\n\n", n * n / 64 > 64 ? " ..." : "");
    }
    ui_pause(NULL);
}

/* ------------------------------------------------- asymptotic notation */
static void notation_demo(void)
{
    ui_header("ASYMPTOTIC NOTATION  (Unit 1)");
    printf("\n  Big O      O(g(n))      : UPPER bound. f(n) <= c*g(n) for n >= n0.\n");
    printf("                            \"the algorithm is never slower than this\"\n");
    printf("\n  Big Omega  Omega(g(n))  : LOWER bound. f(n) >= c*g(n) for n >= n0.\n");
    printf("                            \"the algorithm is never faster than this\"\n");
    printf("\n  Big Theta  Theta(g(n))  : TIGHT bound. Both of the above hold.\n");
    printf("                            \"the algorithm grows exactly like this\"\n");

    printf("\n  Worked example - Linear Search on n elements:\n");
    printf("    best case    : key is first          -> 1 comparison      -> Omega(1)\n");
    printf("    worst case   : key is last or absent -> n comparisons     -> O(n)\n");
    printf("    average case : about n/2 comparisons -> Theta(n)\n");

    printf("\n  Worked example - the loop below:\n");
    printf("      for (i = 0; i < n; i++)          // runs n times\n");
    printf("          for (j = 0; j < n; j++)      // runs n times each\n");
    printf("              c[i][j] = a[i][j] + b[i][j];\n");
    printf("    total operations = n * n = n^2  ->  Theta(n^2)\n");
    printf("    constants and lower order terms are dropped:\n");
    printf("      3n^2 + 5n + 7   ->   O(n^2)\n");

    ui_header("TIME - SPACE TRADE OFF");
    printf("\n  Spending memory can buy speed, and the other way round:\n\n");
    printf("  +-----------------------------+------------+--------------+\n");
    printf("  | situation                   |    time    |    space     |\n");
    printf("  +-----------------------------+------------+--------------+\n");
    printf("  | Linear search, no extra mem |    O(n)    |     O(1)     |\n");
    printf("  | Hash table lookup           |    O(1)    |     O(n)     |\n");
    printf("  | Merge sort (extra array)    | O(n log n) |     O(n)     |\n");
    printf("  | Quick sort (in place)       | O(n log n) |   O(log n)   |\n");
    printf("  | Adjacency matrix            |  O(1) edge |    O(V^2)    |\n");
    printf("  | Adjacency list              |  O(deg) ed |    O(V+E)    |\n");
    printf("  +-----------------------------+------------+--------------+\n");
    ui_pause(NULL);
}

/* ------------------------------------------------- full reference tables */
static void reference_tables(void)
{
    ui_header("COMPLEXITY REFERENCE - DATA STRUCTURE OPERATIONS");
    printf("\n  +----------------------+---------+---------+---------+---------+\n");
    printf("  | structure            | access  | search  | insert  | delete  |\n");
    printf("  +----------------------+---------+---------+---------+---------+\n");
    printf("  | Array (unsorted)     |  O(1)   |  O(n)   |  O(1)*  |  O(n)   |\n");
    printf("  | Array (sorted)       |  O(1)   | O(logn) |  O(n)   |  O(n)   |\n");
    printf("  | Singly linked list   |  O(n)   |  O(n)   |  O(1)#  |  O(1)#  |\n");
    printf("  | Doubly linked list   |  O(n)   |  O(n)   |  O(1)#  |  O(1)#  |\n");
    printf("  | Stack                |  O(1)   |  O(n)   |  O(1)   |  O(1)   |\n");
    printf("  | Queue / Circular Q   |  O(1)   |  O(n)   |  O(1)   |  O(1)   |\n");
    printf("  | Binary Search Tree   |    -    | O(logn) | O(logn) | O(logn) |\n");
    printf("  | BST (degenerate)     |    -    |  O(n)   |  O(n)   |  O(n)   |\n");
    printf("  | AVL Tree             |    -    | O(logn) | O(logn) | O(logn) |\n");
    printf("  | B-Tree               |    -    | O(logn) | O(logn) | O(logn) |\n");
    printf("  | Binary Heap          |  O(1)@  |  O(n)   | O(logn) | O(logn) |\n");
    printf("  | Hash (chaining)      |    -    |  O(1)   |  O(1)   |  O(1)   |\n");
    printf("  | Hash (worst case)    |    -    |  O(n)   |  O(n)   |  O(n)   |\n");
    printf("  +----------------------+---------+---------+---------+---------+\n");
    printf("    *  at the end, if capacity allows      # once the node is located\n");
    printf("    @  the root (min or max) only\n");

    ui_header("COMPLEXITY REFERENCE - ALGORITHMS");
    printf("\n  +----------------------+------------+------------+------------+--------+\n");
    printf("  | algorithm            |    best    |  average   |   worst    | space  |\n");
    printf("  +----------------------+------------+------------+------------+--------+\n");
    printf("  | Linear Search        |    O(1)    |    O(n)    |    O(n)    |  O(1)  |\n");
    printf("  | Binary Search        |    O(1)    |  O(log n)  |  O(log n)  |  O(1)  |\n");
    printf("  | Bubble Sort          |    O(n)    |   O(n^2)   |   O(n^2)   |  O(1)  |\n");
    printf("  | Selection Sort       |   O(n^2)   |   O(n^2)   |   O(n^2)   |  O(1)  |\n");
    printf("  | Insertion Sort       |    O(n)    |   O(n^2)   |   O(n^2)   |  O(1)  |\n");
    printf("  | Merge Sort           | O(n log n) | O(n log n) | O(n log n) |  O(n)  |\n");
    printf("  | Quick Sort           | O(n log n) | O(n log n) |   O(n^2)   | O(logn)|\n");
    printf("  | Heap Sort            | O(n log n) | O(n log n) | O(n log n) |  O(1)  |\n");
    printf("  | Tree traversal       |    O(n)    |    O(n)    |    O(n)    |  O(h)  |\n");
    printf("  | BFS / DFS            |   O(V+E)   |   O(V+E)   |   O(V+E)   |  O(V)  |\n");
    printf("  | Topological sort     |   O(V+E)   |   O(V+E)   |   O(V+E)   |  O(V)  |\n");
    printf("  | Prim (array)         |   O(V^2)   |   O(V^2)   |   O(V^2)   |  O(V)  |\n");
    printf("  | Kruskal              | O(E log E) | O(E log E) | O(E log E) |  O(V)  |\n");
    printf("  | Dijkstra (array)     |   O(V^2)   |   O(V^2)   |   O(V^2)   |  O(V)  |\n");
    printf("  +----------------------+------------+------------+------------+--------+\n");
    printf("    V = vertices, E = edges, h = height of the tree\n");
    printf("    BFS/DFS are O(V^2) in this program because it stores the graph\n");
    printf("    as an adjacency MATRIX, which must be scanned row by row.\n");
    ui_pause(NULL);
}

void complexity_menu(void)
{
    static const char *const items[] = {
        "Asymptotic notation (Big O, Omega, Theta) + time-space trade off",
        "Growth rate table and visual comparison",
        "Full complexity reference tables"
    };
    int choice;
    for (;;) {
        choice = ui_menu("COMPLEXITY ANALYSIS  (Unit 1)", items, 3,
                         "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: notation_demo();   break;
        case 2: growth_table();    break;
        case 3: reference_tables();break;
        default: break;
        }
    }
}

/* ========================================================================= */
/*                            COMPARISON LAB                                 */
/* ========================================================================= */
static void fill_pattern(int a[], int n, int pattern)
{
    int i, j, t;
    for (i = 0; i < n; i++) a[i] = rand() % 90 + 10;
    if (pattern == 1 || pattern == 2) {
        for (i = 1; i < n; i++)
            for (j = i; j > 0 && a[j - 1] > a[j]; j--)
            { t = a[j]; a[j] = a[j - 1]; a[j - 1] = t; }
        if (pattern == 2)
            for (i = 0; i < n / 2; i++)
            { t = a[i]; a[i] = a[n - 1 - i]; a[n - 1 - i] = t; }
    }
    if (pattern == 3) for (i = 0; i < n; i++) a[i] = (rand() % 3 + 1) * 10;
}

static void run_sorting_comparison(const int src[], int n, const char *label)
{
    int id, i;
    int work[MAX_ARRAY];
    Stats st;

    printf("\n  DATA SET : %s   (n = %d)\n", label, n);
    printf("  values   : ");
    for (i = 0; i < n; i++) printf("%d ", src[i]);
    printf("\n");
    printf("\n  +------------------+--------------+----------+----------+-----------+--------+\n");
    printf("  | algorithm        | comparisons  |  swaps   |  moves   |  verdict  | class  |\n");
    printf("  +------------------+--------------+----------+----------+-----------+--------+\n");
    for (id = 1; id <= 6; id++) {
        int ok = 1;
        for (i = 0; i < n; i++) work[i] = src[i];
        stats_reset(&st);
        sort_run(id, work, n, &st, 0);          /* silent run: real counters */
        for (i = 1; i < n; i++) if (work[i - 1] > work[i]) ok = 0;
        printf("  | %-16s | %12ld | %8ld | %8ld | %9s | %-6s |\n",
               sort_name(id), st.comparisons, st.swaps, st.moves,
               ok ? "sorted" : "FAILED",
               (id <= 3) ? "n^2" : "nlogn");
    }
    printf("  +------------------+--------------+----------+----------+-----------+--------+\n");
    printf("    Every number above was counted while the algorithm actually ran\n");
    printf("    on this exact data set - nothing is hard coded.\n");
}

static void sorting_comparison(void)
{
    int a[MAX_ARRAY], n;
    static const char *const items[] = {
        "One data set (you choose it)",
        "Best / average / worst comparison : sorted vs random vs reversed",
        "Effect of input size : n = 5, 10, 20"
    };
    int choice, i;

    choice = ui_menu("SORTING COMPARISON LAB", items, 3, "Back");
    if (choice == 0 || g_eof) return;

    switch (choice) {
    case 1:
        n = ui_build_array(a, MAX_ARRAY, "the comparison");
        ui_header("MEASURED SORTING COMPARISON");
        run_sorting_comparison(a, n, "your data");
        break;
    case 2:
        n = ui_read_int("\n  Array size for all three runs (3-20): ", 3, 20);
        ui_header("MEASURED SORTING COMPARISON - THREE INPUT PATTERNS");
        fill_pattern(a, n, 1); run_sorting_comparison(a, n, "ALREADY SORTED (best case for bubble/insertion)");
        fill_pattern(a, n, 0); run_sorting_comparison(a, n, "RANDOM (average case)");
        fill_pattern(a, n, 2); run_sorting_comparison(a, n, "REVERSE SORTED (worst case)");
        printf("\n  What to notice:\n");
        printf("    - Bubble and Insertion collapse to about n comparisons on\n");
        printf("      sorted data, but blow up to about n^2/2 when reversed.\n");
        printf("    - Selection Sort always does the same n(n-1)/2 comparisons.\n");
        printf("    - Merge and Heap Sort barely move between the three cases.\n");
        printf("    - Quick Sort with a last-element pivot does its WORST on\n");
        printf("      already-sorted data - that is the O(n^2) case.\n");
        break;
    case 3:
        ui_header("MEASURED EFFECT OF INPUT SIZE");
        for (i = 0; i < 3; i++) {
            int sizes[3] = { 5, 10, 20 };
            char lbl[48];
            n = sizes[i];
            fill_pattern(a, n, 0);
            sprintf(lbl, "RANDOM data, n = %d", n);
            run_sorting_comparison(a, n, lbl);
        }
        printf("\n  Doubling n roughly QUADRUPLES the comparison count for the\n");
        printf("  O(n^2) algorithms, while the O(n log n) ones grow far slower.\n");
        break;
    default: break;
    }
    ui_pause(NULL);
}

static void searching_comparison(void)
{
    int a[MAX_ARRAY], n, i, j, t, key;
    Stats sl, sb;

    n = ui_read_int("\n  Array size (5-30): ", 5, MAX_ARRAY);
    for (i = 0; i < n; i++) a[i] = (i + 1) * 5;
    for (i = 1; i < n; i++)
        for (j = i; j > 0 && a[j - 1] > a[j]; j--)
        { t = a[j]; a[j] = a[j - 1]; a[j - 1] = t; }

    ui_header("MEASURED SEARCHING COMPARISON (sorted data)");
    viz_array_plain(a, n);
    printf("\n  +------------------+------------------+------------------+\n");
    printf("  | key searched     | linear comps     | binary comps     |\n");
    printf("  +------------------+------------------+------------------+\n");
    for (i = 0; i < n; i += (n > 8 ? n / 6 : 1)) {
        key = a[i];
        stats_reset(&sl); stats_reset(&sb);
        linear_search(a, n, key, &sl, 0);
        binary_search(a, n, key, &sb, 0);
        printf("  | %-16d | %16ld | %16ld |\n", key, sl.comparisons, sb.comparisons);
    }
    stats_reset(&sl); stats_reset(&sb);
    linear_search(a, n, -1, &sl, 0);
    binary_search(a, n, -1, &sb, 0);
    printf("  | %-16s | %16ld | %16ld |\n", "absent key", sl.comparisons, sb.comparisons);
    printf("  +------------------+------------------+------------------+\n");
    printf("\n  Linear search cost grows with the position of the key.\n");
    printf("  Binary search never exceeds about log2(%d) = %d iterations.\n",
           n, ilog2(n) + 1);
    ui_pause(NULL);
}

static void structure_comparison(void)
{
    ui_header("WHICH STRUCTURE SHOULD I USE?");
    printf("\n  +--------------------------------+-------------------------------+\n");
    printf("  | requirement                    | best choice from this project |\n");
    printf("  +--------------------------------+-------------------------------+\n");
    printf("  | Fast index access a[i]         | Array                         |\n");
    printf("  | Many inserts/deletes in middle | Linked list                   |\n");
    printf("  | Undo, backtracking, recursion  | Stack                         |\n");
    printf("  | Scheduling in arrival order    | Queue                         |\n");
    printf("  | Fixed buffer, reuse the space  | Circular queue                |\n");
    printf("  | Always serve the most urgent   | Priority queue / Heap         |\n");
    printf("  | Sorted data + fast search      | BST (AVL if input is sorted)  |\n");
    printf("  | Huge data on disk              | B-Tree                        |\n");
    printf("  | Near constant time lookup      | Hash table                    |\n");
    printf("  | Networks, maps, dependencies   | Graph                         |\n");
    printf("  +--------------------------------+-------------------------------+\n");

    printf("\n  Same job, three structures - find a value among n items:\n");
    printf("    unsorted array   : O(n)      scan everything\n");
    printf("    balanced BST     : O(log n)  halve the search space each step\n");
    printf("    hash table       : O(1)      compute the index directly\n");
    printf("  The hash table wins on time but spends O(n) memory and loses\n");
    printf("  all ordering information - that is the trade off.\n");
    ui_pause(NULL);
}

void comparison_menu(void)
{
    static const char *const items[] = {
        "Sorting algorithms (measured comparisons / swaps / moves)",
        "Searching algorithms (measured comparisons)",
        "Which data structure to use - a decision table"
    };
    int choice;
    for (;;) {
        choice = ui_menu("ALGORITHM COMPARISON LAB", items, 3, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: sorting_comparison();   break;
        case 2: searching_comparison(); break;
        case 3: structure_comparison(); break;
        default: break;
        }
    }
}
