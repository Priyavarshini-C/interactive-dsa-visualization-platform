/* =========================================================================
 *  graph.c  -  Unit-5 : Graphs
 *              Representation : adjacency matrix, adjacency list, ASCII layout
 *              Traversal      : BFS (queue) and DFS (stack / recursion)
 *              Topological sorting (Kahn's in-degree method)
 *              Minimum spanning tree : Prim's and Kruskal's
 *              Shortest path         : Dijkstra's
 * ========================================================================= */
#include "graph.h"
#include "viz.h"

#define INF 1000000

static int nV        = 0;
static int adjm[MAX_VERTICES][MAX_VERTICES];   /* 0 = no edge, else weight */
static int directed  = 0;
static int weighted  = 0;

/* highlight set used by the layout drawing */
static unsigned char hl_edge[MAX_VERTICES][MAX_VERTICES];
static unsigned char hl_vert[MAX_VERTICES];

static char vname(int i) { return (char)('A' + i); }

/* ---------------------------------------------- tiny sin/cos (no math.h) */
static double t_sin(double x)
{
    const double PI = 3.14159265358979323846;
    double x2, term, sum;
    int i;
    while (x >  PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    x2 = x * x; term = x; sum = x;
    for (i = 1; i < 9; i++) {
        term *= -x2 / ((2 * i) * (2 * i + 1));
        sum  += term;
    }
    return sum;
}
static double t_cos(double x) { return t_sin(x + 1.57079632679489661923); }

/* ------------------------------------------------------------ clear flags */
static void hl_clear(void)
{
    int i, j;
    for (i = 0; i < MAX_VERTICES; i++) {
        hl_vert[i] = 0;
        for (j = 0; j < MAX_VERTICES; j++) hl_edge[i][j] = 0;
    }
}

/* ============================================================ VIEWS */
static void show_matrix(int hlrow)
{
    int i, j;
    printf("\n  ADJACENCY MATRIX  (%s, %s)\n",
           directed ? "directed" : "undirected",
           weighted ? "weighted" : "unweighted");
    printf("       ");
    for (j = 0; j < nV; j++) printf("%4c", vname(j));
    printf("\n      +");
    for (j = 0; j < nV; j++) printf("----");
    printf("-+\n");
    for (i = 0; i < nV; i++) {
        printf("   %c  |", vname(i));
        for (j = 0; j < nV; j++) {
            if (adjm[i][j]) printf("%4d", adjm[i][j]);
            else            printf("%4d", 0);
        }
        printf(" |%s\n", i == hlrow ? "  <- current vertex" : "");
    }
    printf("      +");
    for (j = 0; j < nV; j++) printf("----");
    printf("-+\n");
    printf("    (a non-zero cell is an edge; the number is its weight)\n");
}

static void show_list(void)
{
    int i, j, first;
    printf("\n  ADJACENCY LIST\n");
    for (i = 0; i < nV; i++) {
        printf("    %c | ", vname(i));
        first = 1;
        for (j = 0; j < nV; j++)
            if (adjm[i][j]) {
                if (!first) printf(" -> ");
                if (weighted) printf("[%c,w=%d]", vname(j), adjm[i][j]);
                else          printf("[%c]", vname(j));
                first = 0;
            }
        printf("%s NULL\n", first ? "" : " ->");
    }
    printf("    Space: O(V + E) instead of the matrix's O(V^2).\n");
}

static void show_layout(const char *caption)
{
    const double PI = 3.14159265358979323846;
    int px[MAX_VERTICES], py[MAX_VERTICES], i, j;
    Canvas *cv;
    char lbl[8];

    if (nV == 0) return;
    cv = cv_new(74, 21);
    for (i = 0; i < nV; i++) {
        double ang = -PI / 2.0 + 2.0 * PI * i / nV;
        px[i] = 36 + (int)(30.0 * t_cos(ang));
        py[i] = 10 + (int)( 8.0 * t_sin(ang));
    }
    /* edges first so vertex labels overwrite them */
    for (i = 0; i < nV; i++)
        for (j = 0; j < nV; j++) {
            if (!adjm[i][j]) continue;
            if (!directed && j < i) continue;
            cv_line(cv, px[i], py[i], px[j], py[j],
                    (hl_edge[i][j] || hl_edge[j][i]) ? '#' : '.');
            if (weighted) {
                char w[8];
                sprintf(w, "%d", adjm[i][j]);
                cv_puts(cv, (px[i] + px[j]) / 2, (py[i] + py[j]) / 2 +
                        ((i + j) % 2 ? 1 : -1), w);
            }
        }
    for (i = 0; i < nV; i++) {
        sprintf(lbl, hl_vert[i] ? "(%c)*" : "(%c) ", vname(i));
        cv_puts(cv, px[i] - 1, py[i], lbl);
    }
    printf("\n  %s\n", caption ? caption : "GRAPH LAYOUT");
    cv_print(cv);
    printf("    '.' = edge, '#' = edge used by the algorithm, "
           "'*' = visited vertex\n");
    if (directed) printf("    (arrow direction is given by the matrix, "
                         "not by the drawing)\n");
    cv_free(cv);
}

/* ----------------------------------------------- parent-array tree print */
static void print_tree(const int parent[], int root, int depth)
{
    int i, d;
    for (d = 0; d < depth; d++) printf("    ");
    printf("%s%c\n", depth ? "+-- " : "", vname(root));
    for (i = 0; i < nV; i++)
        if (parent[i] == root) print_tree(parent, i, depth + 1);
}

/* ============================================================ BUILD */
static void sample_undirected(void)
{
    int i, j;
    nV = 6; directed = 0; weighted = 1;
    for (i = 0; i < MAX_VERTICES; i++)
        for (j = 0; j < MAX_VERTICES; j++) adjm[i][j] = 0;
    /*        A  B  C  D  E  F   */
    adjm[0][1] = adjm[1][0] = 4;
    adjm[0][2] = adjm[2][0] = 3;
    adjm[1][2] = adjm[2][1] = 1;
    adjm[1][3] = adjm[3][1] = 2;
    adjm[2][3] = adjm[3][2] = 4;
    adjm[3][4] = adjm[4][3] = 2;
    adjm[2][4] = adjm[4][2] = 5;
    adjm[4][5] = adjm[5][4] = 6;
    adjm[3][5] = adjm[5][3] = 3;
}

static void sample_dag(void)
{
    int i, j;
    nV = 6; directed = 1; weighted = 0;
    for (i = 0; i < MAX_VERTICES; i++)
        for (j = 0; j < MAX_VERTICES; j++) adjm[i][j] = 0;
    adjm[0][1] = 1; adjm[0][2] = 1;
    adjm[1][3] = 1; adjm[2][3] = 1;
    adjm[3][4] = 1; adjm[2][5] = 1; adjm[4][5] = 1;
}

static void sample_disconnected(void)
{
    int i, j;
    nV = 6; directed = 0; weighted = 0;
    for (i = 0; i < MAX_VERTICES; i++)
        for (j = 0; j < MAX_VERTICES; j++) adjm[i][j] = 0;
    adjm[0][1] = adjm[1][0] = 1;
    adjm[1][2] = adjm[2][1] = 1;
    adjm[3][4] = adjm[4][3] = 1;        /* second component */
    /* F (index 5) is isolated */
}

static void graph_create(void)
{
    int i, j, e, ne, u, v, w;
    ui_header("CREATE A GRAPH");
    nV = ui_read_int("\n  Number of vertices (2-10, named A, B, C ...): ",
                     2, MAX_VERTICES);
    directed = ui_yes_no("  Is the graph directed?");
    weighted = ui_yes_no("  Is the graph weighted?");
    for (i = 0; i < MAX_VERTICES; i++)
        for (j = 0; j < MAX_VERTICES; j++) adjm[i][j] = 0;

    ne = ui_read_int("  Number of edges: ", 0, nV * nV);
    for (e = 0; e < ne; e++) {
        char p[96];
        sprintf(p, "    edge %d - from vertex index (0=%c .. %d=%c): ",
                e + 1, vname(0), nV - 1, vname(nV - 1));
        u = ui_read_int(p, 0, nV - 1);
        sprintf(p, "    edge %d - to   vertex index: ", e + 1);
        v = ui_read_int(p, 0, nV - 1);
        if (u == v) { printf("      self loops are ignored in this demo\n"); e--; continue; }
        w = weighted ? ui_read_int("    weight (1-99): ", 1, 99) : 1;
        adjm[u][v] = w;
        if (!directed) adjm[v][u] = w;
    }
    hl_clear();
    show_matrix(-1);
    show_list();
    show_layout("GRAPH YOU ENTERED");
    ui_pause(NULL);
}

/* ============================================================== BFS */
static void bfs_run(void)
{
    int visited[MAX_VERTICES], parent[MAX_VERTICES];
    int queue[MAX_VERTICES], head = 0, tail = 0;
    int i, j, start, order[MAX_VERTICES], on = 0, comp = 0;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    for (i = 0; i < nV; i++) { visited[i] = 0; parent[i] = -1; }
    hl_clear();

    start = ui_read_int("\n  Start vertex index (0 .. n-1): ", 0, nV - 1);
    ui_header("BREADTH FIRST SEARCH  (uses a QUEUE)");

    for (i = 0; i < nV; i++) {
        int s = (i == 0) ? start : i;
        if (visited[s]) continue;
        comp++;
        if (comp > 1)
            printf("\n  --- vertex %c was never reached; the graph is "
                   "DISCONNECTED, restarting BFS here (component %d) ---\n",
                   vname(s), comp);
        visited[s] = 1;
        hl_vert[s] = 1;
        queue[tail++] = s;
        printf("\n  enqueue start vertex %c\n", vname(s));

        while (head < tail) {
            int u = queue[head++];
            order[on++] = u;
            printf("\n  dequeue %c  ->  visit it\n", vname(u));
            printf("    queue now : ");
            if (head == tail) printf("( empty )");
            for (j = head; j < tail; j++) printf("%c ", vname(queue[j]));
            printf("\n    visited   : ");
            for (j = 0; j < nV; j++) printf("%c=%d ", vname(j), visited[j]);
            printf("\n");
            for (j = 0; j < nV; j++) {
                if (adjm[u][j] && !visited[j]) {
                    visited[j] = 1;
                    parent[j]  = u;
                    hl_vert[j] = 1;
                    hl_edge[u][j] = 1;
                    queue[tail++] = j;
                    printf("    neighbour %c is new -> mark visited and enqueue\n",
                           vname(j));
                } else if (adjm[u][j]) {
                    printf("    neighbour %c already visited -> skip\n", vname(j));
                }
            }
            show_layout("BFS PROGRESS");
            ui_step();
        }
    }

    printf("\n  BFS visiting order : ");
    for (i = 0; i < on; i++) printf("%c%s", vname(order[i]), i < on - 1 ? " -> " : "");
    printf("\n\n  BFS SPANNING TREE (parent array)\n");
    for (i = 0; i < nV; i++)
        if (parent[i] == -1) print_tree(parent, i, 0);
    printf("\n  Unweighted shortest hop counts from %c:\n", vname(start));
    {
        int dist[MAX_VERTICES];
        for (i = 0; i < nV; i++) dist[i] = -1;
        dist[start] = 0;
        for (i = 0; i < on; i++) {
            int u = order[i];
            if (parent[u] >= 0 && dist[parent[u]] >= 0) dist[u] = dist[parent[u]] + 1;
        }
        for (i = 0; i < nV; i++) {
            if (dist[i] < 0) printf("    %c : unreachable from %c\n",
                                    vname(i), vname(start));
            else             printf("    %c : %d hop(s)\n", vname(i), dist[i]);
        }
    }
    ui_complexity("BFS", "O(V+E)", "O(V+E)", "O(V+E)",
                  "O(V) for the queue and the visited array",
                  "Adjacency matrix scan makes it O(V^2) here");
    ui_pause(NULL);
}

/* ============================================================== DFS */
static void dfs_run(void)
{
    int visited[MAX_VERTICES], parent[MAX_VERTICES];
    int stack[MAX_VERTICES * MAX_VERTICES], from[MAX_VERTICES * MAX_VERTICES];
    int sp = 0;
    int i, j, start, order[MAX_VERTICES], on = 0, comp = 0;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    for (i = 0; i < nV; i++) { visited[i] = 0; parent[i] = -1; }
    hl_clear();

    start = ui_read_int("\n  Start vertex index (0 .. n-1): ", 0, nV - 1);
    ui_header("DEPTH FIRST SEARCH  (uses a STACK)");

    for (i = 0; i < nV; i++) {
        int s = (i == 0) ? start : i;
        if (visited[s]) continue;
        comp++;
        if (comp > 1)
            printf("\n  --- restarting DFS at %c (component %d, the graph is "
                   "disconnected) ---\n", vname(s), comp);
        stack[sp] = s; from[sp] = -1; sp++;
        while (sp > 0) {
            int u, f;
            sp--;
            u = stack[sp]; f = from[sp];
            if (visited[u]) {
                printf("\n  pop %c  ->  already visited, discard it\n", vname(u));
                continue;
            }
            visited[u] = 1;
            hl_vert[u] = 1;
            order[on++] = u;
            parent[u] = f;                 /* the edge actually used to reach u */
            if (f >= 0) hl_edge[f][u] = 1;
            printf("\n  pop %c  ->  visit it%s\n", vname(u),
                   f >= 0 ? " (reached from its parent)" : "");
            for (j = nV - 1; j >= 0; j--)
                if (adjm[u][j] && !visited[j]) {
                    stack[sp] = j; from[sp] = u; sp++;
                    printf("    push unvisited neighbour %c\n", vname(j));
                }
            printf("    stack (top last) : ");
            if (!sp) printf("( empty )");
            for (j = 0; j < sp; j++) printf("%c ", vname(stack[j]));
            printf("\n");
            show_layout("DFS PROGRESS");
            ui_step();
        }
    }
    printf("\n  DFS visiting order : ");
    for (i = 0; i < on; i++) printf("%c%s", vname(order[i]), i < on - 1 ? " -> " : "");
    printf("\n\n  DFS SPANNING TREE\n");
    for (i = 0; i < nV; i++)
        if (parent[i] == -1) print_tree(parent, i, 0);
    ui_complexity("DFS", "O(V+E)", "O(V+E)", "O(V+E)",
                  "O(V) stack (or recursion depth)",
                  "Goes as deep as possible before backtracking");
    ui_pause(NULL);
}

/* ================================================= TOPOLOGICAL SORTING */
static void topo_run(void)
{
    int indeg[MAX_VERTICES], queue[MAX_VERTICES], head = 0, tail = 0;
    int i, j, out[MAX_VERTICES], on = 0;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    if (!directed) {
        printf("\n  !! Topological sorting is only defined for a DIRECTED "
               "ACYCLIC graph.\n     The current graph is undirected - load the "
               "sample DAG first.\n");
        return;
    }
    ui_header("TOPOLOGICAL SORTING  (Kahn's in-degree method)");
    for (i = 0; i < nV; i++) indeg[i] = 0;
    for (i = 0; i < nV; i++)
        for (j = 0; j < nV; j++) if (adjm[i][j]) indeg[j]++;

    printf("\n  Step 1 : count incoming edges for every vertex\n");
    printf("    +--------+-----------+\n    | vertex | in-degree |\n");
    printf("    +--------+-----------+\n");
    for (i = 0; i < nV; i++) printf("    |   %c    |     %d     |\n", vname(i), indeg[i]);
    printf("    +--------+-----------+\n");

    for (i = 0; i < nV; i++) if (indeg[i] == 0) queue[tail++] = i;
    printf("\n  Step 2 : vertices with in-degree 0 can go first -> queue: ");
    for (i = head; i < tail; i++) printf("%c ", vname(queue[i]));
    printf("\n");

    while (head < tail) {
        int u = queue[head++];
        out[on++] = u;
        printf("\n  take %c into the order\n", vname(u));
        for (j = 0; j < nV; j++)
            if (adjm[u][j]) {
                indeg[j]--;
                printf("    remove edge %c->%c, in-degree(%c) becomes %d%s\n",
                       vname(u), vname(j), vname(j), indeg[j],
                       indeg[j] == 0 ? "  -> enqueue it" : "");
                if (indeg[j] == 0) queue[tail++] = j;
            }
        printf("    order so far : ");
        for (i = 0; i < on; i++) printf("%c ", vname(out[i]));
        printf("\n");
        ui_step();
    }
    if (on < nV) {
        printf("\n  !! Only %d of %d vertices could be ordered.\n", on, nV);
        printf("     The remaining vertices form a CYCLE, so no topological "
               "order exists.\n");
    } else {
        printf("\n  TOPOLOGICAL ORDER : ");
        for (i = 0; i < on; i++) printf("%c%s", vname(out[i]), i < on - 1 ? " -> " : "");
        printf("\n  Every edge points from left to right in this order.\n");
    }
    ui_complexity("Topological sort (Kahn)", "O(V+E)", "O(V+E)", "O(V+E)",
                  "O(V) for the in-degree array and queue",
                  "Only valid for a directed acyclic graph");
    ui_pause(NULL);
}

/* ======================================================== PRIM'S MST */
static void prim_run(void)
{
    int key[MAX_VERTICES], parent[MAX_VERTICES], in[MAX_VERTICES];
    int i, j, total = 0, picked = 0;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    if (directed) { printf("\n  !! MST is defined for undirected graphs.\n"); return; }
    hl_clear();
    ui_header("PRIM'S MINIMUM SPANNING TREE  (grow one tree from a root)");

    for (i = 0; i < nV; i++) { key[i] = INF; parent[i] = -1; in[i] = 0; }
    key[0] = 0;

    for (i = 0; i < nV; i++) {
        int u = -1;
        for (j = 0; j < nV; j++)
            if (!in[j] && (u == -1 || key[j] < key[u])) u = j;
        if (key[u] == INF) {
            printf("\n  !! Vertex %c cannot be reached - the graph is "
                   "disconnected,\n     so a spanning TREE does not exist "
                   "(only a spanning forest).\n", vname(u));
            break;
        }
        in[u] = 1;
        hl_vert[u] = 1;
        if (parent[u] >= 0) {
            hl_edge[parent[u]][u] = 1;
            total += key[u];
            picked++;
            printf("\n  add edge %c - %c  (weight %d)   running cost = %d\n",
                   vname(parent[u]), vname(u), key[u], total);
        } else {
            printf("\n  start the tree at vertex %c\n", vname(u));
        }

        printf("    +--------+-------+--------+--------+\n");
        printf("    | vertex |  key  | parent | in MST |\n");
        printf("    +--------+-------+--------+--------+\n");
        for (j = 0; j < nV; j++) {
            char kb[12];
            if (key[j] == INF) strcpy(kb, "INF");
            else sprintf(kb, "%d", key[j]);
            printf("    |   %c    | %5s | %6c | %6s |\n", vname(j), kb,
                   parent[j] >= 0 ? vname(parent[j]) : '-',
                   in[j] ? "yes" : "no");
        }
        printf("    +--------+-------+--------+--------+\n");

        for (j = 0; j < nV; j++)
            if (adjm[u][j] && !in[j] && adjm[u][j] < key[j]) {
                printf("    edge %c-%c weight %d is cheaper than key[%c]=%s "
                       "-> update\n", vname(u), vname(j), adjm[u][j], vname(j),
                       key[j] == INF ? "INF" : "current");
                key[j]    = adjm[u][j];
                parent[j] = u;
            }
        show_layout("PRIM PROGRESS  ('#' edges are in the MST)");
        ui_step();
    }
    printf("\n  MST edges chosen : %d (a tree on %d vertices needs %d)\n",
           picked, nV, nV - 1);
    printf("  TOTAL MST WEIGHT : %d\n", total);
    ui_complexity("Prim's algorithm", "O(V^2)", "O(V^2) with this array version",
                  "O(V^2)", "O(V) for key/parent/in arrays",
                  "O(E log V) if a binary heap is used instead");
    ui_pause(NULL);
}

/* ===================================================== KRUSKAL'S MST */
typedef struct { int u, v, w; } Edge;

static int uf_parent[MAX_VERTICES];

static int uf_find(int x)
{
    while (uf_parent[x] != x) x = uf_parent[x];
    return x;
}

static void kruskal_run(void)
{
    Edge e[MAX_VERTICES * MAX_VERTICES];
    int ne = 0, i, j, total = 0, picked = 0;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    if (directed) { printf("\n  !! MST is defined for undirected graphs.\n"); return; }
    hl_clear();
    ui_header("KRUSKAL'S MINIMUM SPANNING TREE  (sort edges, union-find)");

    for (i = 0; i < nV; i++)
        for (j = i + 1; j < nV; j++)
            if (adjm[i][j]) { e[ne].u = i; e[ne].v = j; e[ne].w = adjm[i][j]; ne++; }

    for (i = 1; i < ne; i++) {          /* insertion sort by weight */
        Edge k = e[i];
        for (j = i - 1; j >= 0 && e[j].w > k.w; j--) e[j + 1] = e[j];
        e[j + 1] = k;
    }
    printf("\n  Step 1 : all %d edges sorted by increasing weight\n", ne);
    printf("    +------+--------+\n    | edge | weight |\n    +------+--------+\n");
    for (i = 0; i < ne; i++)
        printf("    | %c--%c | %6d |\n", vname(e[i].u), vname(e[i].v), e[i].w);
    printf("    +------+--------+\n");

    for (i = 0; i < nV; i++) uf_parent[i] = i;
    printf("\n  Step 2 : each vertex starts in its own set\n");

    for (i = 0; i < ne && picked < nV - 1; i++) {
        int ru = uf_find(e[i].u), rv = uf_find(e[i].v);
        printf("\n  consider %c--%c (weight %d) : find(%c)=%c  find(%c)=%c\n",
               vname(e[i].u), vname(e[i].v), e[i].w,
               vname(e[i].u), vname(ru), vname(e[i].v), vname(rv));
        if (ru == rv) {
            printf("    same set -> adding it would create a CYCLE, REJECT\n");
        } else {
            uf_parent[ru] = rv;
            total += e[i].w;
            picked++;
            hl_edge[e[i].u][e[i].v] = 1;
            hl_vert[e[i].u] = hl_vert[e[i].v] = 1;
            printf("    different sets -> ACCEPT, union them. cost = %d\n", total);
        }
        printf("    parent array : ");
        for (j = 0; j < nV; j++) printf("%c->%c ", vname(j), vname(uf_parent[j]));
        printf("\n");
        show_layout("KRUSKAL PROGRESS  ('#' edges accepted)");
        ui_step();
    }
    printf("\n  Edges accepted : %d (need %d for a spanning tree)\n",
           picked, nV - 1);
    if (picked < nV - 1)
        printf("  The graph is DISCONNECTED - this is a spanning FOREST, "
               "not a tree.\n");
    printf("  TOTAL MST WEIGHT : %d\n", total);
    ui_complexity("Kruskal's algorithm", "O(E log E)", "O(E log E)",
                  "O(E log E)", "O(V) for the union-find structure",
                  "Dominated by sorting the edges");
    ui_pause(NULL);
}

/* ====================================================== DIJKSTRA'S SP */
static void dijkstra_run(void)
{
    int dist[MAX_VERTICES], done[MAX_VERTICES], parent[MAX_VERTICES];
    int i, j, src, path[MAX_VERTICES], pn;

    if (!nV) { printf("\n  Build a graph first.\n"); return; }
    hl_clear();
    src = ui_read_int("\n  Source vertex index (0 .. n-1): ", 0, nV - 1);
    ui_header("DIJKSTRA'S SHORTEST PATH  (greedy, non-negative weights)");

    for (i = 0; i < nV; i++) { dist[i] = INF; done[i] = 0; parent[i] = -1; }
    dist[src] = 0;

    for (i = 0; i < nV; i++) {
        int u = -1;
        for (j = 0; j < nV; j++)
            if (!done[j] && (u == -1 || dist[j] < dist[u])) u = j;
        if (dist[u] == INF) {
            printf("\n  Remaining vertices are unreachable from %c.\n", vname(src));
            break;
        }
        done[u] = 1;
        hl_vert[u] = 1;
        if (parent[u] >= 0) hl_edge[parent[u]][u] = 1;
        printf("\n  pick the unfinished vertex with the smallest distance: "
               "%c (dist %d)\n", vname(u), dist[u]);

        for (j = 0; j < nV; j++)
            if (adjm[u][j] && !done[j]) {
                int nd = dist[u] + adjm[u][j];
                printf("    relax %c->%c : %d + %d = %d  vs  current %s\n",
                       vname(u), vname(j), dist[u], adjm[u][j], nd,
                       dist[j] == INF ? "INF" : "value");
                if (nd < dist[j]) {
                    dist[j]   = nd;
                    parent[j] = u;
                    printf("      improved -> dist[%c] = %d, via %c\n",
                           vname(j), nd, vname(u));
                } else {
                    printf("      no improvement\n");
                }
            }

        printf("\n    +--------+----------+--------+----------+\n");
        printf("    | vertex | distance | parent | finished |\n");
        printf("    +--------+----------+--------+----------+\n");
        for (j = 0; j < nV; j++) {
            printf("    |   %c    | ", vname(j));
            if (dist[j] == INF) printf("%8s", "INF");
            else                printf("%8d", dist[j]);
            printf(" | %6c | %8s |\n",
                   parent[j] >= 0 ? vname(parent[j]) : '-',
                   done[j] ? "yes" : "no");
        }
        printf("    +--------+----------+--------+----------+\n");
        show_layout("DIJKSTRA PROGRESS");
        ui_step();
    }

    printf("\n  SHORTEST DISTANCES FROM %c\n", vname(src));
    printf("  +--------+----------+--------------------------+\n");
    printf("  | vertex | distance | path                     |\n");
    printf("  +--------+----------+--------------------------+\n");
    for (i = 0; i < nV; i++) {
        printf("  |   %c    | ", vname(i));
        if (dist[i] == INF) { printf("%8s | %-24s |\n", "INF", "unreachable"); continue; }
        printf("%8d | ", dist[i]);
        pn = 0;
        for (j = i; j != -1; j = parent[j]) path[pn++] = j;
        { char line[64] = ""; char t[8];
          for (j = pn - 1; j >= 0; j--) {
              sprintf(t, "%c%s", vname(path[j]), j ? "->" : "");
              strcat(line, t);
          }
          printf("%-24s |\n", line); }
    }
    printf("  +--------+----------+--------------------------+\n");
    ui_complexity("Dijkstra's algorithm", "O(V^2)", "O(V^2) with this array version",
                  "O(V^2)", "O(V) for dist/parent/done",
                  "Fails if any edge weight is negative");
    ui_pause(NULL);
}

/* ================================================================== menu */
void graph_menu(void)
{
    static const char *const items[] = {
        "Create a graph (enter your own vertices and edges)",
        "Load sample : weighted undirected graph (6 vertices)",
        "Load sample : directed acyclic graph for topological sort",
        "Load sample : disconnected graph",
        "Show adjacency matrix / list / ASCII layout",
        "BFS - Breadth First Search",
        "DFS - Depth First Search",
        "Topological sorting",
        "Prim's Minimum Spanning Tree",
        "Kruskal's Minimum Spanning Tree",
        "Dijkstra's Shortest Path"
    };
    int choice;
    for (;;) {
        choice = ui_menu("GRAPH ALGORITHMS  (Unit 5)", items, 11,
                         "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: graph_create(); break;
        case 2: sample_undirected(); hl_clear();
                show_matrix(-1); show_list();
                show_layout("SAMPLE WEIGHTED UNDIRECTED GRAPH");
                ui_pause(NULL); break;
        case 3: sample_dag(); hl_clear();
                show_matrix(-1); show_list();
                show_layout("SAMPLE DIRECTED ACYCLIC GRAPH");
                ui_pause(NULL); break;
        case 4: sample_disconnected(); hl_clear();
                show_matrix(-1); show_list();
                show_layout("SAMPLE DISCONNECTED GRAPH (F is isolated)");
                ui_pause(NULL); break;
        case 5: if (!nV) { printf("\n  Build or load a graph first.\n"); }
                else { hl_clear(); show_matrix(-1); show_list();
                       show_layout("CURRENT GRAPH"); }
                ui_pause(NULL); break;
        case 6:  bfs_run();      break;
        case 7:  dfs_run();      break;
        case 8:  topo_run(); ui_pause(NULL); break;
        case 9:  prim_run();     break;
        case 10: kruskal_run();  break;
        case 11: dijkstra_run(); break;
        default: break;
        }
    }
}
