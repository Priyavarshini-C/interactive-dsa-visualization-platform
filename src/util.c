/* =========================================================================
 *  util.c  -  console utilities, validated input, menu engine
 * ========================================================================= */
#include "common.h"
#include <stdarg.h>
#include <time.h>

int g_interactive = 1;
int g_eof         = 0;

void stats_reset(Stats *s)
{
    s->comparisons = 0;
    s->swaps       = 0;
    s->moves       = 0;
}

void ui_init(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--auto") == 0 || strcmp(argv[i], "-a") == 0)
            g_interactive = 0;          /* no pauses, no screen clearing     */
    }
    srand((unsigned)time(NULL));
}

void *xmalloc(size_t n)
{
    void *p = malloc(n);
    if (!p) {
        fprintf(stderr, "\n*** FATAL: out of memory (%lu bytes) ***\n",
                (unsigned long)n);
        exit(EXIT_FAILURE);
    }
    return p;
}

/* ------------------------------------------------------------------ screen */
void ui_clear(void)
{
    if (!g_interactive) { printf("\n"); return; }
    printf("\033[2J\033[H");            /* ANSI clear; harmless if ignored   */
    fflush(stdout);
}

void ui_rule(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) putchar(c);
    putchar('\n');
}

void ui_title(const char *t)
{
    printf("\n");
    ui_rule('=', 70);
    printf("  %s\n", t);
    ui_rule('=', 70);
}

void ui_header(const char *t)
{
    printf("\n");
    ui_rule('-', 70);
    printf("  %s\n", t);
    ui_rule('-', 70);
}

void ui_note(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf("    ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

/* ------------------------------------------------------------------- input */
static int read_raw(char *buf, size_t n)
{
    if (g_eof) { buf[0] = '\0'; return 0; }
    if (!fgets(buf, (int)n, stdin)) { g_eof = 1; buf[0] = '\0'; return 0; }
    if (!strchr(buf, '\n')) {           /* flush over-long lines             */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) ;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

int ui_read_int(const char *prompt, int lo, int hi)
{
    char buf[MAX_LINE], *end;
    long v;

    for (;;) {
        if (g_eof) return lo;
        printf("%s", prompt);
        fflush(stdout);
        if (!read_raw(buf, sizeof buf)) {
            printf("\n  [end of input - leaving this menu]\n");
            return lo;
        }
        if (buf[0] == '\0') {
            printf("  !! Please type a number between %d and %d.\n", lo, hi);
            continue;
        }
        v = strtol(buf, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (*end != '\0') {
            printf("  !! \"%s\" is not a whole number. Try again.\n", buf);
            continue;
        }
        if (v < lo || v > hi) {
            printf("  !! Out of range. Allowed: %d .. %d.\n", lo, hi);
            continue;
        }
        return (int)v;
    }
}

int ui_read_int_any(const char *prompt)
{
    return ui_read_int(prompt, -999999, 999999);
}

void ui_read_line(const char *prompt, char *buf, size_t n)
{
    printf("%s", prompt);
    fflush(stdout);
    if (!read_raw(buf, n)) buf[0] = '\0';
}

int ui_yes_no(const char *prompt)
{
    char buf[MAX_LINE];
    for (;;) {
        if (g_eof) return 0;
        printf("%s (y/n): ", prompt);
        fflush(stdout);
        if (!read_raw(buf, sizeof buf)) return 0;
        if (buf[0]=='y' || buf[0]=='Y') return 1;
        if (buf[0]=='n' || buf[0]=='N') return 0;
        printf("  !! Please answer y or n.\n");
    }
}

/* ------------------------------------------------------------------ pacing */
void ui_step(void)
{
    char buf[MAX_LINE];
    if (!g_interactive || g_eof) { return; }
    printf("    ... [Enter] next step ");
    fflush(stdout);
    read_raw(buf, sizeof buf);
    printf("\n");
}

void ui_pause(const char *msg)
{
    char buf[MAX_LINE];
    if (!g_interactive || g_eof) { printf("\n"); return; }
    printf("\n%s", msg ? msg : "  [Enter] to continue ");
    fflush(stdout);
    read_raw(buf, sizeof buf);
}

/* ------------------------------------------------------------------- menus */
int ui_menu(const char *title, const char *const *items, int count,
            const char *zero_label)
{
    int i;
    ui_clear();
    ui_title(title);
    for (i = 0; i < count; i++)
        printf("   %2d. %s\n", i + 1, items[i]);
    printf("    0. %s\n", zero_label);
    ui_rule('=', 70);
    return ui_read_int("  Select option: ", 0, count);
}

/* -------------------------------------------------------- complexity panel */
void ui_complexity(const char *algo, const char *best, const char *avg,
                   const char *worst, const char *space, const char *stable)
{
    printf("\n  +--------------------------------------------------------------+\n");
    printf("  | COMPLEXITY : %-47s |\n", algo);
    printf("  +--------------------------------------------------------------+\n");
    printf("  |   Best case    : %-43s |\n", best);
    printf("  |   Average case : %-43s |\n", avg);
    printf("  |   Worst case   : %-43s |\n", worst);
    printf("  |   Space        : %-43s |\n", space);
    if (stable && *stable)
        printf("  |   Note         : %-43s |\n", stable);
    printf("  +--------------------------------------------------------------+\n");
}

/* -------------------------------------------------------- array data entry */
static void fill_random(int a[], int n)
{
    int i;
    for (i = 0; i < n; i++) a[i] = rand() % 90 + 10;   /* 2-digit values */
}

int ui_build_array(int a[], int maxn, const char *what)
{
    static const char *const items[] = {
        "Enter values manually",
        "Random data",
        "Already-sorted data (ascending)",
        "Reverse-sorted data (descending)",
        "Data containing duplicates",
        "Built-in sample set { 42 17 8 31 25 63 4 50 }"
    };
    int choice, n, i, j, tmp;
    char prompt[128];

    sprintf(prompt, "  Data source for %s", what);
    choice = ui_menu(prompt, items, 6, "Cancel (use sample set)");
    if (choice == 0) choice = 6;

    if (choice == 6) {
        int sample[8] = { 42, 17, 8, 31, 25, 63, 4, 50 };
        n = 8;
        for (i = 0; i < n; i++) a[i] = sample[i];
        return n;
    }

    n = ui_read_int("  How many elements? ", 1, maxn);

    switch (choice) {
    case 1:
        for (i = 0; i < n; i++) {
            char p[64];
            sprintf(p, "    element[%d] = ", i);
            a[i] = ui_read_int(p, -9999, 9999);
        }
        break;
    case 2:
        fill_random(a, n);
        break;
    case 3:
        fill_random(a, n);
        for (i = 1; i < n; i++)            /* insertion sort ascending */
            for (j = i; j > 0 && a[j-1] > a[j]; j--)
            { tmp = a[j]; a[j] = a[j-1]; a[j-1] = tmp; }
        break;
    case 4:
        fill_random(a, n);
        for (i = 1; i < n; i++)
            for (j = i; j > 0 && a[j-1] < a[j]; j--)
            { tmp = a[j]; a[j] = a[j-1]; a[j-1] = tmp; }
        break;
    case 5:
        for (i = 0; i < n; i++) a[i] = (rand() % 3 + 1) * 10;
        break;
    default:
        fill_random(a, n);
        break;
    }
    return n;
}
