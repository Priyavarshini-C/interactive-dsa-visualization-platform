#ifndef SEARCHING_H
#define SEARCHING_H
#include "common.h"

/* Core algorithms. verbose = 1 draws the step-by-step visualisation.
 * Both return the index of key, or -1 when absent.                         */
int linear_search(const int a[], int n, int key, Stats *st, int verbose);
int binary_search(const int a[], int n, int key, Stats *st, int verbose);

void searching_menu(void);
#endif
