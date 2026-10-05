#ifndef SORTING_H
#define SORTING_H
#include "common.h"

/* Every routine sorts a[0..n-1] ascending, records work in *st and,
 * when verbose != 0, renders each significant step.                         */
void bubble_sort   (int a[], int n, Stats *st, int verbose);
void selection_sort(int a[], int n, Stats *st, int verbose);
void insertion_sort(int a[], int n, Stats *st, int verbose);
void merge_sort    (int a[], int n, Stats *st, int verbose);
void quick_sort    (int a[], int n, Stats *st, int verbose);
void heap_sort     (int a[], int n, Stats *st, int verbose);

/* name/complexity table used by the comparison lab */
const char *sort_name(int id);              /* id 1..6 */
void        sort_run (int id, int a[], int n, Stats *st, int verbose);
void        sort_complexity(int id);

void sorting_menu(void);
#endif
