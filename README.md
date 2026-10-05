# Interactive Algorithm Simulation and Visualization Platform

**Course:** Data Structures and Algorithms (21CSC201J)
**Programme:** B.Tech Computer Science and Engineering, SRMIST (2021 Regulations)
**Language:** Standard C (C99). No external libraries, no `math.h`, no HTML/CSS/JS/Python/C++.
**Interface:** Interactive terminal menus with ASCII visualisation.

---

## 1. Build and run

### Using make

```
make            # builds ./dsaviz
make run        # builds and runs
make clean      # removes the binary
```

### Single compilation command (works anywhere)

```
gcc -std=c99 -Wall -Iinclude src/*.c -o dsaviz
```

On Windows with MinGW:

```
gcc -std=c99 -Wall -Iinclude src\*.c -o dsaviz.exe
```

No `-lm` is needed — the sine/cosine used by the graph layout is computed
from a Taylor series inside `graph.c`, so the project depends on nothing
outside the C standard library.

### Execution

```
./dsaviz           # normal mode: pauses after each algorithm step
./dsaviz --auto    # no pauses and no screen clearing (used for automated tests)
```

On Windows: `dsaviz.exe`

---

## 2. Scope — driven by the actual syllabus

Every module below maps to a unit of the official 21CSC201J course plan.
Topics outside the syllabus were deliberately **not** included.

| Unit | Syllabus content | Where it is implemented |
|------|------------------|-------------------------|
| 1 | ADT, Big O / Omega / Theta, time–space trade off, complexity | `analysis.c` (menu 10) |
| 1 | Searching and sorting with complexity (CLR-1, CO-2) | `searching.c`, `sorting.c` (menus 1, 2) |
| 2 | List ADT: array, cursor-based, linked; singly, doubly, circular | `list.c` (menu 3) |
| 2 | Applications: sparse matrix, polynomial arithmetic, Josephus | `list.c` (menu 3) |
| 3 | Stack ADT: array and linked | `stack.c` (menu 4) |
| 3 | Applications: balancing symbols, infix→postfix, postfix evaluation, Tower of Hanoi | `stack.c` (menu 4) |
| 3 | Queue ADT: array and linked; circular, double ended, priority | `queue.c` (menu 5) |
| 4 | Tree traversals, complete binary tree and its height, BST | `tree.c` (menu 6) |
| 4 | Need for balance, rotation, AVL trees | `tree.c` (menu 6) |
| 4 | B-Trees | `tree.c` (menu 6) |
| 4 | Heaps, array implementation and applications | `heap.c` (menu 7) |
| 4 | Hash functions, collision avoidance, separate chaining, open addressing, linear and quadratic probing | `hashing.c` (menu 8) |
| 5 | Graph representation and traversal | `graph.c` (menu 9) |
| 5 | Topological sorting | `graph.c` (menu 9) |
| 5 | Minimum spanning tree: Prim's and Kruskal's | `graph.c` (menu 9) |
| 5 | Shortest path: Dijkstra's | `graph.c` (menu 9) |

---

## 3. File layout

```
include/            src/
  common.h            main.c        entry point and main menu
  viz.h               util.c        validated input, menus, banners
  searching.h         viz.c         ASCII renderers (arrays, bars, canvas, trees, chains)
  sorting.h           searching.c   linear and binary search
  list.h              sorting.c     bubble, selection, insertion, merge, quick, heap sort
  stack.h             list.c        singly/doubly/circular/cursor lists + 3 applications
  queue.h             stack.c       array and linked stack + 4 applications
  tree.h              queue.c       linear, circular, linked, deque, priority queue
  heap.h              tree.c        BST, AVL, B-Tree
  hashing.h           heap.c        max/min binary heap, heapify, priority queue
  graph.h             hashing.c     chaining, linear probing, quadratic probing
  analysis.h          graph.c       BFS, DFS, topological sort, Prim, Kruskal, Dijkstra
                      analysis.c    complexity reference + measured comparison lab
Makefile
README.md
```

The separation is deliberate: `viz.c` holds every drawing routine, so each
algorithm file contains only the algorithm plus the calls that hand its
current state to the renderer. Nothing is drawn from stored or faked data.

---

## 4. Visualisation primitives

| Renderer | Used by |
|----------|---------|
| Boxed array with `=` emphasis borders, arrow markers and labels | searching, sorting, heap |
| Vertical bar chart | sorting |
| Character canvas with Bresenham line drawing | graph layout |
| Top-down binary tree with `+---+---+` connectors | BST, AVL, heap |
| Linked chain with `--->`, `<-->` and circular loop-back wire | lists, stack, queue |
| Vertical stack boxes | stack |
| Peg-and-disk drawing | Tower of Hanoi |
| Slot tables with front/rear markers | queue, hashing |

---

## 5. Test results

Every claim below was produced by actually running the program.

**Sorting — all six algorithms verified correct on every data pattern.**
Counters are incremented inside the algorithms, never hard coded.

Sample set `42 17 8 31 25 63 4 50` (n = 8):

| Algorithm | Comparisons | Swaps | Moves | Verdict |
|---|---|---|---|---|
| Bubble Sort | 28 | 13 | 0 | sorted |
| Selection Sort | 28 | 5 | 0 | sorted |
| Insertion Sort | 17 | 0 | 20 | sorted |
| Merge Sort | 16 | 0 | 24 | sorted |
| Quick Sort | 20 | 6 | 0 | sorted |
| Heap Sort | 27 | 21 | 0 | sorted |

Best/average/worst at n = 10 confirmed the theory:
Bubble 9 → 44 → 45 comparisons (early exit works on sorted data);
Selection a constant 45 = n(n−1)/2 in all three cases;
Insertion 9 → 25 → 45; Merge Sort's moves fixed at 34 regardless of order;
Quick Sort hits its 45-comparison worst case on **already sorted** data,
which is the expected weakness of a last-element pivot.
With heavy duplicates all six still sorted correctly.

**Searching.** Linear and binary search verified for present and absent keys;
binary search never exceeded ⌈log₂ n⌉ + 1 iterations.

**BST.** Sample tree `50 30 70 20 40 60 80` gives
inorder `20 30 40 50 60 70 80` (sorted, as required),
preorder `50 30 20 40 70 60 80`,
postorder `20 40 30 60 80 70 50`,
level order `50 30 70 20 40 60 80`.
All three deletion cases exercised in sequence — leaf (20), one child (30),
two children (50, replaced by inorder successor 60) — producing the correct
final tree.

**AVL.** Insertion order `50 40 30 60 70 10 20 55 52` triggers all four
rebalancing cases exactly once each: LL, RR, LR and RL (verified by counting
the rotation messages). Nine keys stored at height 4 instead of a degenerate chain.

**B-Tree.** The CLRS example `10 20 5 6 12 30 7 17` with minimum degree t = 2
produces root `[10|20]` over children `[5|6|7] [12|17] [30]`, and the inorder
listing returns `5 6 7 10 12 17 20 30`.

**Heap.** Bottom-up build on the sample set gives the valid max-heap
`63 50 42 31 25 8 4 17`; draining the priority queue returns
`63 50 42 31 25 17 8 4` in descending order.

**Hashing.** Keys `12 25 36 20 30 42 17` with m = 11.
Separate chaining: 2 collisions, α = 0.64, chains correct.
Linear probing: 36 settles at index 4 after 2 probes, 42 at index 10 after 2 probes;
searching 42 correctly follows the probe chain 9 → 10 and reports a match.
Deletion marks a slot `DELETED` rather than `EMPTY` so probe chains stay intact.

**Graph** (sample: 6 vertices, 9 weighted undirected edges).
BFS from A: order `A B C D E F`, hop counts A=0, B=1, C=1, D=2, E=2, F=3.
DFS from A: order `A B C D E F` with a genuinely deep spanning tree A→B→C→D→E→F.
Prim's MST total weight **11**; Kruskal's MST total weight **11** — two independent
algorithms agreeing is a strong cross-check. Kruskal correctly rejects nothing
here and accepts exactly 5 edges for 6 vertices.
Dijkstra from A: A=0, B=4, C=3, D=6, E=8, F=9 with reconstructed paths
(`F` via `A→B→D→F`). Topological sort on the sample DAG yields `A B C D E F`.
On the disconnected sample, BFS and DFS restart per component and report the
unreachable vertices, and Kruskal reports a spanning **forest** (3 edges, not 5).

**Stack applications.**
`{a+[b*(c-d)]}` → balanced; `{a+[b)}` → not balanced, position 5 reported.
`a+b*(c-d)/e` → `abcd-*e/+`; `a^b^c` → `abc^^` (right associative);
`(a+b)*(c-d)` → `ab+cd-*`. Postfix `23*54*+9-` evaluates to **17**.
Tower of Hanoi with 3 disks solved in 7 moves = 2³ − 1.

**Queue.** The linear queue reports a false overflow while two slots are still
free; the circular queue reuses those slots and only reports full when all six
are occupied — the exact contrast the syllabus asks for.

**List applications.** Josephus with n = 7, k = 3 eliminates
`3 6 2 7 5 1` and leaves survivor **4**, matching the textbook result.
Polynomial addition: `3x^2 + 1` plus `4x^2 + 5x` gives `7x^2 + 5x + 1`.

**Robustness.** Verified with no crash and a clear message for: non-numeric
input (`abc`), a decimal (`2.7`), empty input, out-of-range and negative menu
choices, popping an empty stack, dequeuing an empty queue, pushing onto a full
stack, deleting a value that is not present, searching for a missing key,
an empty tree, a single-element array, duplicate values, already-sorted and
reverse-sorted data, and a disconnected graph.
Reaching end-of-input unwinds every menu level, frees allocated memory and
exits with status 0.

**Memory.** Built with `-fsanitize=address,undefined` and exercised across all
eleven modules: **zero** memory errors, zero undefined-behaviour reports and
zero leaks. LeakSanitizer was confirmed active by a deliberate control leak.

---

## 6. Likely viva questions this project answers

- Why does a linear queue overflow while slots are free, and how does the
  circular queue fix it? — menu 5, options 1 and 2 side by side.
- Why does a BST need balancing? — menu 6 → AVL → option 5 shows the same keys
  in a degenerate BST and a balanced AVL tree.
- Why is Quick Sort O(n²) in the worst case? — menu 11 → option 2, where
  already-sorted data produces the worst count.
- What is the difference between separate chaining and open addressing, and why
  is a deleted slot marked rather than emptied? — menu 8.
- Why is build-heap O(n) but repeated insertion O(n log n)? — menu 7, option 4.
- Prim versus Kruskal on the same graph — menu 9, options 9 and 10.

---

## 7. Assumptions made

1. **No reference ZIP was supplied**, so the whole project was built from
   scratch against the syllabus rather than adapted from existing code.
2. Visualisation uses **plain ASCII only** (`+ - | ^ # = . *`), not Unicode box
   characters, so it renders identically in Windows `cmd`, PowerShell and Linux
   terminals without code-page problems.
3. Display sizes are capped so diagrams stay readable: arrays up to 30 elements
   (15 in step-by-step sorting), graphs up to 10 vertices, array stack and queue
   capacity 6–8, hash table size 11 (prime), B-Tree minimum degree t = 2,
   Tower of Hanoi up to 6 disks. The algorithms themselves have no such limits.
4. **B-Tree deletion** and **AVL deletion** are not implemented. The syllabus
   lists B-Trees and AVL rotations under Unit 4 and Lab 12 covers B-Tree
   implementation; insertion with splitting and the four rotation cases cover
   the examinable behaviour, while their deletion algorithms are long enough to
   obscure the teaching point. Insertion, search, traversal and display are complete.
5. Graphs are stored as an **adjacency matrix** because the syllabus Lab 13 asks
   for "Implementation of Graph using Array". The adjacency list is also rendered
   for comparison. This makes BFS/DFS O(V²) here rather than O(V+E), which the
   complexity module states explicitly rather than hiding.
6. Dijkstra and Prim use the **array (linear scan) selection**, giving O(V²),
   which is the standard form taught at this level; the heap-based O(E log V)
   variant is named in the complexity panel.
7. Postfix evaluation accepts **single-digit operands**, the usual classroom
   convention, so the parsing does not distract from the stack algorithm.
8. The ANSI escape used to clear the screen is skipped in `--auto` mode and is
   harmless on terminals that ignore it.
