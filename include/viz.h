/* =========================================================================
 *  viz.h  -  ASCII visualisation primitives.
 *
 *  Nothing here invents data: every renderer draws exactly the state that
 *  the calling algorithm passes to it.
 * ========================================================================= */
#ifndef VIZ_H
#define VIZ_H

/* ---------------- boxed array rendering ---------------------------------- */
typedef struct {
    int         idx;        /* index the arrow points at    */
    const char *label;      /* text printed under the arrow */
} Marker;

/* emph may be NULL; otherwise emph[i]!=0 draws cell i with '=' borders. */
void viz_array(const int a[], int n, const unsigned char *emph,
               const Marker *mk, int nmk);

/* convenience wrappers */
void viz_array_plain(const int a[], int n);
void viz_array_1(const int a[], int n, int i, const char *li);
void viz_array_2(const int a[], int n, int i, const char *li,
                                       int j, const char *lj);
void viz_array_3(const int a[], int n, int i, const char *li,
                                       int j, const char *lj,
                                       int k, const char *lk);

/* vertical bar chart (skipped automatically if any value is negative) */
void viz_bars(const int a[], int n, const unsigned char *emph);

/* ---------------- character canvas ---------------------------------------- */
typedef struct {
    int   w, h;
    char *b;
} Canvas;

Canvas *cv_new(int w, int h);
void    cv_free(Canvas *c);
void    cv_put(Canvas *c, int x, int y, char ch);
void    cv_puts(Canvas *c, int x, int y, const char *s);
void    cv_line(Canvas *c, int x0, int y0, int x1, int y1, char ch);
void    cv_print(Canvas *c);

/* ---------------- generic binary tree renderer ---------------------------- */
typedef struct VTree {
    char          label[20];
    int           emph;             /* 1 -> label drawn as *label* */
    struct VTree *l, *r;
    int           x, y;             /* filled in by viz_tree */
} VTree;

VTree *vt_new(const char *label, int emph);
void   vt_free(VTree *t);
void   viz_tree(VTree *root);       /* draws a top-down ASCII tree */
void   viz_heap_tree(const int a[], int n, int emph1, int emph2);

/* linked-structure renderer: style 0=singly 1=doubly 2=circular */
void   viz_chain(const int v[], int n, int style, int hl, const char *head_label);

#endif /* VIZ_H */
