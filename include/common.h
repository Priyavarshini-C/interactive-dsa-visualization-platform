/* =========================================================================
 *  common.h  -  Shared utilities for the Interactive Algorithm Simulation
 *               and Visualization Platform.
 *
 *  Safe console input, menu rendering, banners and step pausing.
 * ========================================================================= */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ARRAY     30     /* largest array the visualiser renders well   */
#define MAX_VERTICES  10     /* largest graph the ASCII layout renders well */
#define MAX_LINE      256

/* ---- global run-time flags ---------------------------------------------- */
extern int g_interactive;    /* 1 = pause between steps, clear before menus  */
extern int g_eof;            /* 1 = stdin exhausted, unwind all menus        */

/* ---- instrumentation shared by sorting / searching / comparison ---------- */
typedef struct {
    long comparisons;
    long swaps;
    long moves;              /* element writes (merge sort, insertion sort)  */
} Stats;

void stats_reset(Stats *s);

/* ---- program start-up ---------------------------------------------------- */
void ui_init(int argc, char **argv);

/* ---- screen furniture ---------------------------------------------------- */
void ui_clear(void);
void ui_rule(char c, int n);
void ui_title(const char *t);          /* big double-ruled banner            */
void ui_header(const char *t);         /* small section header               */
void ui_note(const char *fmt, ...);    /* indented explanatory line          */

/* ---- pacing -------------------------------------------------------------- */
void ui_step(void);                    /* "press Enter for next step"        */
void ui_pause(const char *msg);

/* ---- input (all validated, never trusts scanf) --------------------------- */
int  ui_read_int(const char *prompt, int lo, int hi);
int  ui_read_int_any(const char *prompt);
void ui_read_line(const char *prompt, char *buf, size_t n);
int  ui_yes_no(const char *prompt);

/* ---- menus --------------------------------------------------------------- */
int  ui_menu(const char *title, const char *const *items, int count,
             const char *zero_label);

/* ---- data entry helpers -------------------------------------------------- */
/* Offers manual / random / sorted / reverse / duplicate / sample data sets.
 * Returns the number of elements written into a[].                           */
int  ui_build_array(int a[], int maxn, const char *what);

/* ---- misc ---------------------------------------------------------------- */
void *xmalloc(size_t n);
void  ui_complexity(const char *algo, const char *best, const char *avg,
                    const char *worst, const char *space, const char *stable);

#endif /* COMMON_H */
