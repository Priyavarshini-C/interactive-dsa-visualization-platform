/* =========================================================================
 *  INTERACTIVE ALGORITHM SIMULATION AND VISUALIZATION PLATFORM
 *  Data Structures and Algorithms (21CSC201J) - B.Tech CSE, SRMIST
 *
 *  main.c  -  entry point and top level menu
 *
 *  Build :  make          (or see README.md for the single gcc line)
 *  Run   :  ./dsaviz            (step-by-step, pauses for Enter)
 *           ./dsaviz --auto     (no pauses - useful for piped test input)
 * ========================================================================= */
#include "common.h"
#include "searching.h"
#include "sorting.h"
#include "list.h"
#include "stack.h"
#include "queue.h"
#include "tree.h"
#include "heap.h"
#include "hashing.h"
#include "graph.h"
#include "analysis.h"

static void splash(void)
{
    ui_clear();
    printf("\n");
    ui_rule('=', 70);
    printf("                                                                      \n");
    printf("        INTERACTIVE ALGORITHM SIMULATION AND VISUALIZATION             \n");
    printf("                           P L A T F O R M                            \n");
    printf("                                                                      \n");
    printf("        Data Structures and Algorithms  -  21CSC201J                   \n");
    printf("        Implemented in standard C with ASCII terminal graphics         \n");
    printf("                                                                      \n");
    ui_rule('=', 70);
    printf("\n  Every diagram in this program is generated from the real state of\n");
    printf("  the running algorithm - nothing is pre-recorded.\n");
    if (g_interactive)
        printf("\n  Tip: run with  --auto  to skip the step-by-step pauses.\n");
    ui_pause("\n  [Enter] to open the main menu ");
}

int main(int argc, char **argv)
{
    static const char *const items[] = {
        "Searching Algorithms          (linear, binary)",
        "Sorting Algorithms            (bubble, selection, insertion, merge, quick, heap)",
        "List Structures               (singly, doubly, circular, cursor + applications)",
        "Stack                         (array, linked + 4 applications)",
        "Queue                         (linear, circular, linked, deque, priority)",
        "Trees                         (BST, AVL, B-Tree, traversals)",
        "Heap / Priority Queue         (max-heap, min-heap, heapify)",
        "Hashing                       (chaining, linear and quadratic probing)",
        "Graph Algorithms              (BFS, DFS, topological sort, Prim, Kruskal, Dijkstra)",
        "Complexity Analysis           (Big O, Omega, Theta, growth rates)",
        "Algorithm Comparison Lab      (measured statistics)"
    };
    int choice;

    ui_init(argc, argv);
    splash();

    for (;;) {
        choice = ui_menu("INTERACTIVE DSA VISUALIZATION PLATFORM - MAIN MENU",
                         items, 11, "Exit");
        if (g_eof && choice == 0) break;
        switch (choice) {
        case 1:  searching_menu();  break;
        case 2:  sorting_menu();    break;
        case 3:  list_menu();       break;
        case 4:  stack_menu();      break;
        case 5:  queue_menu();      break;
        case 6:  tree_menu();       break;
        case 7:  heap_menu();       break;
        case 8:  hashing_menu();    break;
        case 9:  graph_menu();      break;
        case 10: complexity_menu(); break;
        case 11: comparison_menu(); break;
        case 0:
            ui_clear();
            ui_title("GOODBYE");
            printf("\n  All dynamically allocated memory has been released by the\n");
            printf("  individual modules before returning to this menu.\n\n");
            return 0;
        default: break;
        }
    }
    printf("\n  Input stream closed - exiting.\n");
    return 0;
}
