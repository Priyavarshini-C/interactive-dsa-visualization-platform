/* =========================================================================
 *  hashing.c  -  Unit-4 : Hashing
 *                h(k) = k mod m  (m prime)
 *                Collision resolution :
 *                   - Separate chaining  (linked lists per bucket)
 *                   - Open addressing    : linear probing
 *                                        : quadratic probing
 * ========================================================================= */
#include "hashing.h"
#include "viz.h"

#define HSIZE 11                   /* a prime keeps the spread even */

/* ====================================================== SEPARATE CHAINING */
typedef struct HNode { int key; struct HNode *next; } HNode;

static HNode *bucket[HSIZE];
static long   chain_collisions;

static void chain_clear(void)
{
    int i; HNode *p, *t;
    for (i = 0; i < HSIZE; i++) {
        p = bucket[i];
        while (p) { t = p->next; free(p); p = t; }
        bucket[i] = NULL;
    }
    chain_collisions = 0;
}

static void chain_show(int hl)
{
    int i, count = 0;
    HNode *p;
    printf("\n  HASH TABLE - SEPARATE CHAINING   (m = %d, h(k) = k mod %d)\n",
           HSIZE, HSIZE);
    printf("  +--------+--------------------------------------------------+\n");
    printf("  | bucket | chain                                            |\n");
    printf("  +--------+--------------------------------------------------+\n");
    for (i = 0; i < HSIZE; i++) {
        char line[128] = "";
        char tmp[24];
        for (p = bucket[i]; p; p = p->next) {
            sprintf(tmp, "[%d]->", p->key);
            if (strlen(line) + strlen(tmp) < sizeof(line) - 8) strcat(line, tmp);
            count++;
        }
        strcat(line, "NULL");
        printf("  | %s%4d%s | %-48s |\n",
               i == hl ? ">" : " ", i, i == hl ? "<" : " ", line);
    }
    printf("  +--------+--------------------------------------------------+\n");
    printf("    keys stored = %d   load factor alpha = %.2f   collisions = %ld\n",
           count, (double)count / HSIZE, chain_collisions);
}

static void chaining_menu(void)
{
    static const char *const items[] = {
        "Insert a key", "Search a key", "Delete a key", "Display table",
        "Clear table", "Load sample keys 12 25 36 20 30 42 17"
    };
    int choice, k, i;
    HNode *p, *prev;

    for (;;) {
        choice = ui_menu("HASHING - SEPARATE CHAINING", items, 6,
                         "Back (frees all chains)");
        if (choice == 0 || g_eof) { chain_clear(); return; }
        switch (choice) {
        case 1:
            k = ui_read_int("  Key to insert: ", 0, 9999);
            { int idx = k % HSIZE;
              printf("\n  h(%d) = %d mod %d = %d\n", k, k, HSIZE, idx);
              if (bucket[idx]) {
                  chain_collisions++;
                  printf("  bucket %d is already occupied -> COLLISION\n", idx);
                  printf("  the new key is pushed at the FRONT of that chain\n");
              } else {
                  printf("  bucket %d is empty -> direct placement\n", idx);
              }
              p = (HNode *)xmalloc(sizeof(HNode));
              p->key = k; p->next = bucket[idx]; bucket[idx] = p;
              chain_show(idx); }
            break;
        case 2:
            k = ui_read_int("  Key to search: ", 0, 9999);
            { int idx = k % HSIZE, probes = 0, found = 0;
              printf("\n  h(%d) = %d -> walk the chain in bucket %d\n", k, idx, idx);
              for (p = bucket[idx]; p; p = p->next) {
                  probes++;
                  printf("    compare with %d\n", p->key);
                  if (p->key == k) { found = 1; break; }
              }
              chain_show(idx);
              printf("\n  RESULT : %d is %s after %d comparison(s).\n",
                     k, found ? "PRESENT" : "ABSENT", probes); }
            break;
        case 3:
            k = ui_read_int("  Key to delete: ", 0, 9999);
            { int idx = k % HSIZE, done = 0;
              prev = NULL;
              for (p = bucket[idx]; p; prev = p, p = p->next)
                  if (p->key == k) {
                      if (prev) prev->next = p->next; else bucket[idx] = p->next;
                      free(p); done = 1; break;
                  }
              printf("\n  %s\n", done ? "deleted - the chain was relinked"
                                      : "key not found in its bucket");
              chain_show(idx); }
            break;
        case 4: chain_show(-1); break;
        case 5: chain_clear(); printf("\n  Table cleared.\n"); break;
        case 6: {
            int keys[7] = { 12, 25, 36, 20, 30, 42, 17 };
            chain_clear();
            for (i = 0; i < 7; i++) {
                int idx = keys[i] % HSIZE;
                if (bucket[idx]) chain_collisions++;
                p = (HNode *)xmalloc(sizeof(HNode));
                p->key = keys[i]; p->next = bucket[idx]; bucket[idx] = p;
                printf("  insert %2d -> bucket %d%s\n", keys[i], idx,
                       p->next ? "   (collision, chained)" : "");
            }
            chain_show(-1);
            break;
        }
        default: break;
        }
        ui_complexity("Separate chaining", "O(1)", "O(1 + alpha)",
                      "O(n) - every key in one chain",
                      "O(n) for the nodes + O(m) for the bucket array",
                      "Table can never 'fill up'");
        ui_pause(NULL);
    }
}

/* ======================================================== OPEN ADDRESSING */
#define EMPTY   -1
#define DELETED -2

static int  slot[HSIZE];
static long oa_collisions;
static int  quadratic;              /* 0 = linear, 1 = quadratic */

static void oa_clear(void)
{
    int i;
    for (i = 0; i < HSIZE; i++) slot[i] = EMPTY;
    oa_collisions = 0;
}

static void oa_show(int hl)
{
    int i, used = 0;
    printf("\n  HASH TABLE - %s PROBING   (m = %d)\n",
           quadratic ? "QUADRATIC" : "LINEAR", HSIZE);
    printf("  +-------+----------+\n");
    printf("  | index |   key    |\n");
    printf("  +-------+----------+\n");
    for (i = 0; i < HSIZE; i++) {
        printf("  | %s%3d%s | ", i == hl ? ">" : " ", i, i == hl ? "<" : " ");
        if (slot[i] == EMPTY)        printf("%8s |\n", "empty");
        else if (slot[i] == DELETED) printf("%8s |\n", "DELETED");
        else { printf("%8d |\n", slot[i]); used++; }
    }
    printf("  +-------+----------+\n");
    printf("    keys = %d / %d    load factor alpha = %.2f    collisions = %ld\n",
           used, HSIZE, (double)used / HSIZE, oa_collisions);
}

static int oa_probe(int k, int i)
{
    return quadratic ? (k % HSIZE + i * i) % HSIZE
                     : (k % HSIZE + i)     % HSIZE;
}

static void oa_menu(void)
{
    static const char *const items[] = {
        "Insert a key", "Search a key", "Delete a key", "Display table",
        "Clear table", "Switch linear / quadratic",
        "Load sample keys 12 25 36 20 30 42 17"
    };
    int choice, k, i, idx;

    oa_clear();
    for (;;) {
        char title[80];
        sprintf(title, "HASHING - OPEN ADDRESSING (%s probing)",
                quadratic ? "quadratic" : "linear");
        choice = ui_menu(title, items, 7, "Back");
        if (choice == 0 || g_eof) return;

        switch (choice) {
        case 1:
            k = ui_read_int("  Key to insert: ", 0, 9999);
            printf("\n  h(%d) = %d mod %d = %d\n", k, k, HSIZE, k % HSIZE);
            for (i = 0; i < HSIZE; i++) {
                idx = oa_probe(k, i);
                if (quadratic)
                    printf("    probe %d : (h + %d^2) mod %d = %d -> ", i, i, HSIZE, idx);
                else
                    printf("    probe %d : (h + %d) mod %d = %d -> ", i, i, HSIZE, idx);
                if (slot[idx] == EMPTY || slot[idx] == DELETED) {
                    printf("free, key placed here\n");
                    slot[idx] = k;
                    break;
                }
                if (slot[idx] == k) { printf("key already present\n"); break; }
                printf("occupied by %d -> COLLISION, probe again\n", slot[idx]);
                oa_collisions++;
            }
            if (i == HSIZE)
                printf("    !! no free slot reachable - table full (or the "
                       "quadratic sequence cannot reach the rest)\n");
            oa_show(i < HSIZE ? idx : -1);
            break;
        case 2:
            k = ui_read_int("  Key to search: ", 0, 9999);
            { int found = -1;
              for (i = 0; i < HSIZE; i++) {
                  idx = oa_probe(k, i);
                  printf("    probe %d -> index %d : ", i, idx);
                  if (slot[idx] == EMPTY) { printf("empty, stop - key is absent\n");
                                            break; }
                  if (slot[idx] == k)     { printf("MATCH\n"); found = idx; break; }
                  printf("holds %s, keep probing\n",
                         slot[idx] == DELETED ? "a DELETED marker" : "another key");
              }
              oa_show(found);
              printf("\n  RESULT : %d is %s\n", k,
                     found >= 0 ? "PRESENT" : "ABSENT"); }
            break;
        case 3:
            k = ui_read_int("  Key to delete: ", 0, 9999);
            { int done = 0;
              for (i = 0; i < HSIZE; i++) {
                  idx = oa_probe(k, i);
                  if (slot[idx] == EMPTY) break;
                  if (slot[idx] == k) { slot[idx] = DELETED; done = 1; break; }
              }
              printf("\n  %s\n", done
                     ? "slot marked DELETED, not EMPTY - otherwise later probe\n"
                       "  chains would break and existing keys would become "
                       "unreachable."
                     : "key not found");
              oa_show(done ? idx : -1); }
            break;
        case 4: oa_show(-1); break;
        case 5: oa_clear(); printf("\n  Table cleared.\n"); break;
        case 6:
            quadratic = !quadratic;
            oa_clear();
            printf("\n  Switched to %s probing (table cleared).\n",
                   quadratic ? "QUADRATIC" : "LINEAR");
            if (quadratic)
                ui_note("Quadratic probing spreads clusters out, but may not "
                        "reach every slot.");
            else
                ui_note("Linear probing is simple but suffers from PRIMARY "
                        "CLUSTERING.");
            break;
        case 7: {
            int keys[7] = { 12, 25, 36, 20, 30, 42, 17 };
            oa_clear();
            for (k = 0; k < 7; k++) {
                int key = keys[k];
                for (i = 0; i < HSIZE; i++) {
                    idx = oa_probe(key, i);
                    if (slot[idx] == EMPTY) { slot[idx] = key; break; }
                    oa_collisions++;
                }
                printf("  insert %2d : h=%d, settled at index %d after %d probe(s)\n",
                       key, key % HSIZE, idx, i + 1);
            }
            oa_show(-1);
            break;
        }
        default: break;
        }
        ui_complexity(quadratic ? "Quadratic probing" : "Linear probing",
                      "O(1)", "O(1/(1-alpha)) expected",
                      "O(n) when the table is nearly full",
                      "O(m) - one fixed array, no pointers",
                      "Keep alpha below about 0.7");
        ui_pause(NULL);
    }
}

/* ========================================================= hash functions */
static void hashfn_demo(void)
{
    int k, i;
    ui_header("HASH FUNCTIONS");
    ui_note("A hash function maps a key to an index in 0 .. m-1.");
    ui_note("A good one spreads keys evenly and is cheap to compute.");
    k = ui_read_int("\n  Enter a key: ", 0, 99999);
    printf("\n  +---------------------+-------------------------------+-------+\n");
    printf("  | method              | computation                   | index |\n");
    printf("  +---------------------+-------------------------------+-------+\n");
    printf("  | Division            | %d mod %-2d                   | %5d |\n",
           k, HSIZE, k % HSIZE);
    {
        double f = (double)k * 0.6180339887;
        int    mid;
        f = f - (int)f;
        printf("  | Multiplication      | floor(%d*0.618 frac * %d)      | %5d |\n",
               k, HSIZE, (int)(f * HSIZE));
        mid = (k * k / 10) % HSIZE;
        printf("  | Mid-square          | middle digits of %d^2        | %5d |\n",
               k, mid);
    }
    {
        int sum = 0, t = k;
        while (t) { sum += t % 10; t /= 10; }
        printf("  | Digit folding       | digit sum %d mod %d           | %5d |\n",
               sum, HSIZE, sum % HSIZE);
    }
    printf("  +---------------------+-------------------------------+-------+\n");

    printf("\n  Spread of the division method over 0..20 for m = %d:\n\n", HSIZE);
    printf("    key   : ");
    for (i = 0; i <= 20; i++) printf("%3d", i);
    printf("\n    index : ");
    for (i = 0; i <= 20; i++) printf("%3d", i % HSIZE);
    printf("\n");
    ui_note("m is prime so that patterned keys do not all land on one bucket.");
    ui_pause(NULL);
}

/* ================================================================== menu */
void hashing_menu(void)
{
    static const char *const items[] = {
        "Hash functions (division, multiplication, mid-square, folding)",
        "Collision handling : Separate chaining",
        "Collision handling : Open addressing (linear / quadratic probing)"
    };
    int choice;
    for (;;) {
        choice = ui_menu("HASHING  (Unit 4)", items, 3, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: hashfn_demo();   break;
        case 2: chaining_menu(); break;
        case 3: oa_menu();       break;
        default: break;
        }
    }
}
