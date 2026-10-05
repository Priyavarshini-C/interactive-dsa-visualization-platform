/* =========================================================================
 *  viz.c  -  ASCII visualisation primitives
 * ========================================================================= */
#include "common.h"
#include "viz.h"

/* ====================================================== boxed array views */

static int cell_width(const int a[], int n)
{
    int i, len, maxlen = 1;
    char tmp[32];
    for (i = 0; i < n; i++) {
        sprintf(tmp, "%d", a[i]);
        len = (int)strlen(tmp);
        if (len > maxlen) maxlen = len;
    }
    return maxlen;            /* digits only; cell = digits + 2 spaces */
}

/* place s on row centred at col; return 0 when it would collide */
static int place_text(char *row, int width, int centre, const char *s)
{
    int len = (int)strlen(s);
    int start = centre - len / 2, i;
    if (start < 0) start = 0;
    if (start + len > width) start = width - len;
    if (start < 0) return 0;
    for (i = start - 1; i <= start + len; i++)
        if (i >= 0 && i < width && row[i] != ' ') return 0;
    for (i = 0; i < len; i++) row[start + i] = s[i];
    return 1;
}

void viz_array(const int a[], int n, const unsigned char *emph,
               const Marker *mk, int nmk)
{
    int digits, cw, total, i, j, m, placed, rows;
    char *line;
    char buf[32];

    if (n <= 0) {
        printf("\n    ( empty - no elements to display )\n");
        return;
    }
    digits = cell_width(a, n);
    cw     = digits + 2;
    total  = n * (cw + 1) + 1;
    line   = (char *)xmalloc((size_t)total + 2);

    /* ---- index row ---- */
    memset(line, ' ', (size_t)total);
    line[total] = '\0';
    for (i = 0; i < n; i++) {
        sprintf(buf, "%d", i);
        place_text(line, total, i * (cw + 1) + 1 + cw / 2, buf);
    }
    printf("  %s\n", line);

    /* ---- top border ---- */
    printf("  ");
    for (i = 0; i < n; i++) {
        char fill = (emph && emph[i]) ? '=' : '-';
        putchar('+');
        for (j = 0; j < cw; j++) putchar(fill);
    }
    printf("+\n");

    /* ---- value row ---- */
    printf("  ");
    for (i = 0; i < n; i++) printf("| %*d ", digits, a[i]);
    printf("|\n");

    /* ---- bottom border ---- */
    printf("  ");
    for (i = 0; i < n; i++) {
        char fill = (emph && emph[i]) ? '=' : '-';
        putchar('+');
        for (j = 0; j < cw; j++) putchar(fill);
    }
    printf("+\n");

    /* ---- arrow row ---- */
    if (nmk > 0) {
        int any = 0;
        memset(line, ' ', (size_t)total);
        line[total] = '\0';
        for (m = 0; m < nmk; m++) {
            if (mk[m].idx < 0 || mk[m].idx >= n) continue;
            line[mk[m].idx * (cw + 1) + 1 + cw / 2] = '^';
            any = 1;
        }
        if (any) printf("  %s\n", line);

        /* ---- label rows (wrap to a new row on collision) ---- */
        placed = 0;
        for (rows = 0; rows < 4 && placed < nmk; rows++) {
            int wrote = 0;
            memset(line, ' ', (size_t)total);
            line[total] = '\0';
            for (m = 0; m < nmk; m++) {
                if (mk[m].idx < 0 || mk[m].idx >= n || !mk[m].label) continue;
                if (place_text(line, total,
                               mk[m].idx * (cw + 1) + 1 + cw / 2,
                               mk[m].label))
                    wrote = 1;
            }
            if (!wrote) break;
            printf("  %s\n", line);
            /* mark the ones drawn in this row as done */
            {
                int remaining = 0;
                for (m = 0; m < nmk; m++) {
                    if (mk[m].idx < 0 || mk[m].idx >= n || !mk[m].label)
                        continue;
                    remaining++;
                }
                placed = remaining;   /* single pass is enough in practice */
            }
            break;
        }
    }
    free(line);
}

void viz_array_plain(const int a[], int n)
{
    viz_array(a, n, NULL, NULL, 0);
}

void viz_array_1(const int a[], int n, int i, const char *li)
{
    Marker mk[1];
    unsigned char e[MAX_ARRAY];
    int t;
    for (t = 0; t < n && t < MAX_ARRAY; t++) e[t] = (unsigned char)(t == i);
    mk[0].idx = i; mk[0].label = li;
    viz_array(a, n, e, mk, 1);
}

void viz_array_2(const int a[], int n, int i, const char *li,
                                       int j, const char *lj)
{
    Marker mk[2];
    unsigned char e[MAX_ARRAY];
    int t;
    for (t = 0; t < n && t < MAX_ARRAY; t++)
        e[t] = (unsigned char)(t == i || t == j);
    mk[0].idx = i; mk[0].label = li;
    mk[1].idx = j; mk[1].label = lj;
    viz_array(a, n, e, mk, 2);
}

void viz_array_3(const int a[], int n, int i, const char *li,
                                       int j, const char *lj,
                                       int k, const char *lk)
{
    Marker mk[3];
    unsigned char e[MAX_ARRAY];
    int t;
    for (t = 0; t < n && t < MAX_ARRAY; t++)
        e[t] = (unsigned char)(t == i || t == j || t == k);
    mk[0].idx = i; mk[0].label = li;
    mk[1].idx = j; mk[1].label = lj;
    mk[2].idx = k; mk[2].label = lk;
    viz_array(a, n, e, mk, 3);
}

/* ============================================================ bar chart */

void viz_bars(const int a[], int n, const unsigned char *emph)
{
    const int H = 10;
    int i, lvl, maxv = 0, digits, cw;
    int scaled[MAX_ARRAY];

    if (n <= 0 || n > MAX_ARRAY) return;
    for (i = 0; i < n; i++) {
        if (a[i] < 0) return;                 /* bar chart needs a >= 0 */
        if (a[i] > maxv) maxv = a[i];
    }
    if (maxv == 0) return;

    digits = cell_width(a, n);
    cw     = digits + 2;
    for (i = 0; i < n; i++) {
        scaled[i] = (a[i] * H + maxv - 1) / maxv;
        if (a[i] > 0 && scaled[i] == 0) scaled[i] = 1;
    }
    for (lvl = H; lvl >= 1; lvl--) {
        printf("  ");
        for (i = 0; i < n; i++) {
            char c = (emph && emph[i]) ? '*' : '#';
            int  j;
            putchar(' ');
            for (j = 0; j < cw; j++)
                putchar(scaled[i] >= lvl ? c : ' ');
        }
        printf("\n");
    }
}

/* =============================================================== canvas */

Canvas *cv_new(int w, int h)
{
    Canvas *c = (Canvas *)xmalloc(sizeof(Canvas));
    c->w = w; c->h = h;
    c->b = (char *)xmalloc((size_t)w * (size_t)h);
    memset(c->b, ' ', (size_t)w * (size_t)h);
    return c;
}

void cv_free(Canvas *c)
{
    if (!c) return;
    free(c->b);
    free(c);
}

void cv_put(Canvas *c, int x, int y, char ch)
{
    if (!c || x < 0 || y < 0 || x >= c->w || y >= c->h) return;
    c->b[y * c->w + x] = ch;
}

void cv_puts(Canvas *c, int x, int y, const char *s)
{
    int i;
    for (i = 0; s[i]; i++) cv_put(c, x + i, y, s[i]);
}

void cv_line(Canvas *c, int x0, int y0, int x1, int y1, char ch)
{
    int dx = x1 - x0, dy = y1 - y0;
    int sx = dx > 0 ? 1 : -1, sy = dy > 0 ? 1 : -1;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int err, e2, x = x0, y = y0;

    err = (adx > ady ? adx : -ady) / 2;
    for (;;) {
        if (!(x == x0 && y == y0) && !(x == x1 && y == y1)) {
            char cur = (x >= 0 && y >= 0 && x < c->w && y < c->h)
                       ? c->b[y * c->w + x] : 'X';
            if (cur == ' ' || cur == '-' || cur == '|' ||
                cur == '/' || cur == '\\' || cur == ch)
                cv_put(c, x, y, ch);
        }
        if (x == x1 && y == y1) break;
        e2 = err;
        if (e2 > -adx) { err -= ady; x += sx; }
        if (e2 <  ady) { err += adx; y += sy; }
    }
}

void cv_print(Canvas *c)
{
    int x, y, last;
    for (y = 0; y < c->h; y++) {
        last = -1;
        for (x = 0; x < c->w; x++)
            if (c->b[y * c->w + x] != ' ') last = x;
        printf("  ");
        for (x = 0; x <= last; x++) putchar(c->b[y * c->w + x]);
        putchar('\n');
    }
}

/* ======================================================== binary trees */

VTree *vt_new(const char *label, int emph)
{
    VTree *t = (VTree *)xmalloc(sizeof(VTree));
    strncpy(t->label, label, sizeof(t->label) - 1);
    t->label[sizeof(t->label) - 1] = '\0';
    t->emph = emph;
    t->l = t->r = NULL;
    t->x = t->y = 0;
    return t;
}

void vt_free(VTree *t)
{
    if (!t) return;
    vt_free(t->l);
    vt_free(t->r);
    free(t);
}

static void vt_disp(const VTree *t, char *out, size_t n)
{
    if (t->emph) snprintf(out, n, "*%s*", t->label);
    else         snprintf(out, n, "%s", t->label);
}

static void vt_measure(const VTree *t, int *maxlen, int *count, int depth,
                       int *maxdepth)
{
    char buf[32];
    int  len;
    if (!t) return;
    vt_measure(t->l, maxlen, count, depth + 1, maxdepth);
    vt_disp(t, buf, sizeof buf);
    len = (int)strlen(buf);
    if (len > *maxlen)  *maxlen  = len;
    if (depth > *maxdepth) *maxdepth = depth;
    (*count)++;
    vt_measure(t->r, maxlen, count, depth + 1, maxdepth);
}

static void vt_assign(VTree *t, int cw, int depth, int *counter)
{
    if (!t) return;
    vt_assign(t->l, cw, depth + 1, counter);
    t->x = (*counter) * cw + cw / 2;
    t->y = depth * 2;
    (*counter)++;
    vt_assign(t->r, cw, depth + 1, counter);
}

static void vt_draw(VTree *t, Canvas *cv)
{
    char buf[32];
    int  len, c, lx, rx;
    if (!t) return;
    vt_disp(t, buf, sizeof buf);
    len = (int)strlen(buf);
    cv_puts(cv, t->x - len / 2, t->y, buf);

    if (t->l || t->r) {
        lx = t->l ? t->l->x : t->x;
        rx = t->r ? t->r->x : t->x;
        for (c = lx; c <= rx; c++) cv_put(cv, c, t->y + 1, '-');
        if (t->l) cv_put(cv, lx, t->y + 1, '+');
        if (t->r) cv_put(cv, rx, t->y + 1, '+');
        cv_put(cv, t->x, t->y + 1, '+');
    }
    vt_draw(t->l, cv);
    vt_draw(t->r, cv);
}

static void vt_indent(const VTree *t, int depth, const char *tag)
{
    char buf[32];
    int  i;
    if (!t) return;
    vt_disp(t, buf, sizeof buf);
    for (i = 0; i < depth; i++) printf("    ");
    printf("%s%s\n", tag, buf);
    vt_indent(t->l, depth + 1, "L: ");
    vt_indent(t->r, depth + 1, "R: ");
}

void viz_tree(VTree *root)
{
    int maxlen = 1, count = 0, maxdepth = 0, cw, counter = 0;
    Canvas *cv;

    if (!root) {
        printf("\n    ( tree is empty )\n");
        return;
    }
    vt_measure(root, &maxlen, &count, 0, &maxdepth);
    cw = maxlen + 2;
    if (count * cw > 150) {               /* too wide for a terminal */
        printf("\n    ( tree too wide to draw - indented view instead )\n");
        vt_indent(root, 0, "");
        return;
    }
    vt_assign(root, cw, 0, &counter);
    cv = cv_new(count * cw + 2, maxdepth * 2 + 1);
    vt_draw(root, cv);
    printf("\n");
    cv_print(cv);
    cv_free(cv);
}

/* ---- build a VTree mirror of an array-based (implicit) binary tree ------ */
static VTree *vt_from_array(const int a[], int n, int i, int emph1, int emph2)
{
    char buf[20];
    VTree *t;
    if (i >= n) return NULL;
    sprintf(buf, "%d", a[i]);
    t = vt_new(buf, (i == emph1 || i == emph2));
    t->l = vt_from_array(a, n, 2 * i + 1, emph1, emph2);
    t->r = vt_from_array(a, n, 2 * i + 2, emph1, emph2);
    return t;
}

void viz_heap_tree(const int a[], int n, int emph1, int emph2)
{
    VTree *t;
    if (n <= 0) { printf("\n    ( heap is empty )\n"); return; }
    t = vt_from_array(a, n, 0, emph1, emph2);
    viz_tree(t);
    vt_free(t);
}

/* ======================================================= linked structures */
/* style: 0 = singly (--->), 1 = doubly (<-->), 2 = circular (loops back)    */
void viz_chain(const int v[], int n, int style, int hl, const char *head_label)
{
    int i, j, digits, bw, lead, total;
    char tmp[32];

    if (n <= 0) {
        printf("\n    %s -> NULL        ( list is empty )\n",
               head_label ? head_label : "HEAD");
        return;
    }
    digits = 1;
    for (i = 0; i < n; i++) {
        sprintf(tmp, "%d", v[i]);
        if ((int)strlen(tmp) > digits) digits = (int)strlen(tmp);
    }
    bw   = digits + 4;                       /* box inner width */
    lead = (style == 1) ? 8 : 0;             /* room for "NULL <--" */

    printf("\n  %s\n", head_label ? head_label : "HEAD");
    printf("   |\n   v\n");

    /* top border */
    printf("  ");
    for (j = 0; j < lead; j++) putchar(' ');
    for (i = 0; i < n; i++) {
        char f = (i == hl) ? '=' : '-';
        putchar('+');
        for (j = 0; j < bw; j++) putchar(f);
        putchar('+');
        if (i < n - 1) printf("    ");
    }
    printf("\n");

    /* data row */
    printf("  ");
    if (style == 1) printf("NULL <--");
    for (i = 0; i < n; i++) {
        printf("|%*d%*s|", (bw + digits) / 2, v[i],
               bw - (bw + digits) / 2, "");
        if (i < n - 1) printf(style == 1 ? "<-->" : "--->");
    }
    if (style == 0) printf("---> NULL");
    else if (style == 1) printf("--> NULL");
    else printf("--+");
    printf("\n");

    /* bottom border */
    printf("  ");
    for (j = 0; j < lead; j++) putchar(' ');
    for (i = 0; i < n; i++) {
        char f = (i == hl) ? '=' : '-';
        putchar('+');
        for (j = 0; j < bw; j++) putchar(f);
        putchar('+');
        if (i < n - 1) printf("    ");
    }
    if (style == 2) printf("  |");
    printf("\n");

    if (style == 2) {                        /* circular loop-back wire */
        total = n * (bw + 2) + (n - 1) * 4;
        printf("  ");
        for (j = 0; j < (bw + 2) / 2; j++) putchar(' ');
        putchar('^');
        for (j = (bw + 2) / 2 + 1; j < total + 2; j++) putchar(' ');
        printf("|\n  ");
        for (j = 0; j < (bw + 2) / 2; j++) putchar(' ');
        putchar('+');
        for (j = (bw + 2) / 2 + 1; j < total + 2; j++) putchar('-');
        printf("+\n");
    }
}
