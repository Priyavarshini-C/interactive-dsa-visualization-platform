/* =========================================================================
 *  stack.c  -  Unit-3 : Stack ADT
 *              Implementations : array based and linked
 *              Applications    : balancing symbols, infix -> postfix,
 *                                postfix evaluation, Tower of Hanoi,
 *                                function-call (recursion) stack
 * ========================================================================= */
#include "stack.h"
#include "viz.h"

#define STACK_CAP 8

/* ------------------------------------------------ vertical stack drawing */
static void draw_stack_int(const int v[], int top, int cap, const char *title)
{
    int i;
    printf("\n  %s   (capacity %d, top index %d, size %d)\n",
           title, cap, top, top + 1);
    if (top < 0) {
        printf("        +--------+\n");
        printf("        |  EMPTY |   <- TOP = -1 (underflow if popped)\n");
        printf("        +--------+\n");
        printf("           BASE\n");
        return;
    }
    for (i = top; i >= 0; i--) {
        if (i == top) printf("  TOP -> ");
        else          printf("         ");
        printf("+--------+\n");
        if (i == top) printf("         |%6d  |  <- last pushed\n", v[i]);
        else          printf("         |%6d  |\n", v[i]);
    }
    printf("         +--------+\n");
    printf("            BASE\n");
    if (top == cap - 1) printf("    (stack is FULL - another push would overflow)\n");
}

static void draw_stack_char(const char v[], int top, const char *title)
{
    int i;
    printf("\n  %s  (size %d)\n", title, top + 1);
    if (top < 0) { printf("         +-----+\n         |EMPTY|\n         +-----+\n"); return; }
    for (i = top; i >= 0; i--) {
        printf(i == top ? "  TOP -> " : "         ");
        printf("+-----+\n         |  %c  |\n", v[i]);
    }
    printf("         +-----+\n");
}

/* ============================================================ ARRAY STACK */
static void array_stack_menu(void)
{
    static const char *const items[] = {
        "Push", "Pop", "Peek / Top", "Display", "Is empty?", "Is full?"
    };
    int st[STACK_CAP], top = -1, choice, x;

    for (;;) {
        choice = ui_menu("STACK ADT - ARRAY IMPLEMENTATION", items, 6, "Back");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1:
            draw_stack_int(st, top, STACK_CAP, "BEFORE PUSH");
            if (top == STACK_CAP - 1) {
                printf("\n  !! STACK OVERFLOW - the array is full, push rejected.\n");
                break;
            }
            x = ui_read_int("  Value to push: ", -9999, 9999);
            st[++top] = x;
            printf("\n  top = top + 1;  stack[top] = %d;\n", x);
            draw_stack_int(st, top, STACK_CAP, "AFTER PUSH");
            break;
        case 2:
            draw_stack_int(st, top, STACK_CAP, "BEFORE POP");
            if (top < 0) {
                printf("\n  !! STACK UNDERFLOW - nothing to pop.\n");
                break;
            }
            x = st[top--];
            printf("\n  popped %d;  top = top - 1;\n", x);
            draw_stack_int(st, top, STACK_CAP, "AFTER POP");
            break;
        case 3:
            if (top < 0) printf("\n  Stack is empty - no top element.\n");
            else { printf("\n  TOP element = %d (not removed)\n", st[top]);
                   draw_stack_int(st, top, STACK_CAP, "STACK"); }
            break;
        case 4: draw_stack_int(st, top, STACK_CAP, "STACK"); break;
        case 5: printf("\n  isEmpty() -> %s\n", top < 0 ? "TRUE" : "FALSE"); break;
        case 6: printf("\n  isFull()  -> %s\n",
                       top == STACK_CAP - 1 ? "TRUE" : "FALSE"); break;
        default: break;
        }
        ui_complexity("Stack (array)", "O(1)", "O(1)", "O(1)",
                      "O(n) for n slots, fixed at compile time",
                      "push / pop / peek are all constant time");
        ui_pause(NULL);
    }
}

/* =========================================================== LINKED STACK */
typedef struct LNode { int data; struct LNode *next; } LNode;

static void linked_stack_show(LNode *top)
{
    int v[MAX_ARRAY], n = 0, i;
    LNode *p = top;
    while (p && n < MAX_ARRAY) { v[n++] = p->data; p = p->next; }
    printf("\n  TOP of stack is the HEAD of the list:\n");
    viz_chain(v, n, 0, 0, "TOP");
    printf("\n  Same stack drawn vertically:\n");
    if (n == 0) { printf("         ( empty )\n"); return; }
    for (i = 0; i < n; i++) {
        printf(i == 0 ? "  TOP -> " : "         ");
        printf("+--------+\n         |%6d  |\n", v[i]);
    }
    printf("         +--------+\n            NULL\n");
}

static void linked_stack_menu(void)
{
    static const char *const items[] = { "Push", "Pop", "Peek / Top", "Display" };
    LNode *top = NULL, *p;
    int choice, x;

    for (;;) {
        choice = ui_menu("STACK ADT - LINKED IMPLEMENTATION", items, 4,
                         "Back (frees all nodes)");
        if (choice == 0 || g_eof) {
            while (top) { p = top->next; free(top); top = p; }
            return;
        }
        switch (choice) {
        case 1:
            x = ui_read_int("  Value to push: ", -9999, 9999);
            linked_stack_show(top);
            p = (LNode *)xmalloc(sizeof(LNode));
            p->data = x; p->next = top; top = p;
            printf("\n  malloc a node, link it in front of the old top.\n");
            printf("  No overflow unless malloc() itself fails.\n");
            linked_stack_show(top);
            break;
        case 2:
            if (!top) { printf("\n  !! STACK UNDERFLOW - the list is empty.\n"); break; }
            linked_stack_show(top);
            p = top; x = p->data; top = top->next; free(p);
            printf("\n  popped %d, node freed.\n", x);
            linked_stack_show(top);
            break;
        case 3:
            if (!top) printf("\n  Stack is empty.\n");
            else printf("\n  TOP element = %d\n", top->data);
            break;
        case 4: linked_stack_show(top); break;
        default: break;
        }
        ui_pause(NULL);
    }
}

/* ================================================ APPLICATION : BALANCING */
static void balancing_demo(void)
{
    char expr[MAX_LINE], st[MAX_LINE];
    int top = -1, i, ok = 1, badpos = -1;

    ui_header("APPLICATION : BALANCING SYMBOLS  ( ) [ ] { }");
    ui_read_line("\n  Enter an expression: ", expr, sizeof expr);
    if (expr[0] == '\0') strcpy(expr, "{a+[b*(c-d)]}");
    printf("  Expression: %s\n", expr);

    for (i = 0; expr[i]; i++) {
        char c = expr[i];
        if (c=='(' || c=='[' || c=='{') {
            st[++top] = c;
            printf("\n  read '%c' at position %d -> opening symbol, PUSH\n", c, i);
            draw_stack_char(st, top, "SYMBOL STACK");
            ui_step();
        } else if (c==')' || c==']' || c=='}') {
            char want = (c==')') ? '(' : (c==']') ? '[' : '{';
            printf("\n  read '%c' at position %d -> closing symbol, POP and match\n", c, i);
            if (top < 0) {
                printf("    stack is EMPTY -> unmatched closing symbol\n");
                ok = 0; badpos = i; break;
            }
            if (st[top] != want) {
                printf("    top is '%c' but '%c' was expected -> MISMATCH\n",
                       st[top], want);
                ok = 0; badpos = i; break;
            }
            printf("    top '%c' matches -> pop it\n", st[top]);
            top--;
            draw_stack_char(st, top, "SYMBOL STACK");
            ui_step();
        }
    }
    printf("\n  ------------------------------------------------\n");
    if (ok && top == -1) printf("  RESULT: the expression is BALANCED.\n");
    else if (ok)         printf("  RESULT: NOT balanced - %d opening symbol(s) "
                                "left on the stack.\n", top + 1);
    else                 printf("  RESULT: NOT balanced - problem at position %d.\n",
                                badpos);
    ui_complexity("Balancing symbols", "O(n)", "O(n)", "O(n)",
                  "O(n) worst case stack", "One left-to-right scan");
    ui_pause(NULL);
}

/* ========================================= APPLICATION : INFIX -> POSTFIX */
static int precedence(char op)
{
    switch (op) {
    case '^': return 3;
    case '*': case '/': case '%': return 2;
    case '+': case '-': return 1;
    default: return 0;
    }
}

static void infix_to_postfix(const char *in, char *out)
{
    char st[MAX_LINE];
    int top = -1, i, k = 0;

    printf("\n  +-------+--------------------------+--------------------------+\n");
    printf("  | token | stack (bottom -> top)    | postfix so far           |\n");
    printf("  +-------+--------------------------+--------------------------+\n");

    for (i = 0; in[i]; i++) {
        char c = in[i];
        if (c == ' ') continue;
        if (isalnum((unsigned char)c)) {
            out[k++] = c;
        } else if (c == '(') {
            st[++top] = c;
        } else if (c == ')') {
            while (top >= 0 && st[top] != '(') out[k++] = st[top--];
            if (top >= 0) top--;                       /* discard '(' */
        } else if (precedence(c)) {
            while (top >= 0 && st[top] != '(' &&
                   (precedence(st[top]) > precedence(c) ||
                    (precedence(st[top]) == precedence(c) && c != '^')))
                out[k++] = st[top--];
            st[++top] = c;
        } else {
            continue;                                  /* ignore junk */
        }
        out[k] = '\0';
        { char sbuf[MAX_LINE]; int t;
          for (t = 0; t <= top; t++) sbuf[t] = st[t];
          sbuf[top + 1] = '\0';
          printf("  |   %c   | %-24s | %-24s |\n", c, sbuf, out); }
    }
    while (top >= 0) {
        out[k++] = st[top--];
        out[k] = '\0';
        printf("  |  pop  | %-24s | %-24s |\n", "", out);
    }
    out[k] = '\0';
    printf("  +-------+--------------------------+--------------------------+\n");
}

static void infix_demo(void)
{
    char in[MAX_LINE], out[MAX_LINE];
    ui_header("APPLICATION : INFIX TO POSTFIX CONVERSION");
    ui_note("Operands go straight to the output; operators wait on the stack");
    ui_note("until an operator of lower or equal precedence arrives.");
    ui_read_line("\n  Infix expression (e.g. a+b*(c-d)/e ): ", in, sizeof in);
    if (in[0] == '\0') strcpy(in, "a+b*(c-d)/e");
    printf("  Infix   : %s\n", in);
    infix_to_postfix(in, out);
    printf("\n  POSTFIX : %s\n", out);
    ui_complexity("Infix to postfix", "O(n)", "O(n)", "O(n)",
                  "O(n) operator stack", "Each token is pushed/popped once");
    ui_pause(NULL);
}

/* ======================================= APPLICATION : POSTFIX EVALUATION */
static void postfix_demo(void)
{
    char in[MAX_LINE];
    int st[MAX_LINE], top = -1, i, a, b, err = 0;

    ui_header("APPLICATION : POSTFIX EVALUATION");
    ui_note("Single-digit operands. Operators: + - * / %");
    ui_read_line("\n  Postfix expression (e.g. 23*54*+9- ): ", in, sizeof in);
    if (in[0] == '\0') strcpy(in, "23*54*+9-");
    printf("  Postfix : %s\n", in);

    for (i = 0; in[i]; i++) {
        char c = in[i];
        if (c == ' ') continue;
        if (isdigit((unsigned char)c)) {
            st[++top] = c - '0';
            printf("\n  '%c' is an operand -> push %d\n", c, c - '0');
        } else if (strchr("+-*/%", c)) {
            if (top < 1) { printf("\n  !! malformed expression at '%c'\n", c);
                           err = 1; break; }
            b = st[top--]; a = st[top--];
            switch (c) {
            case '+': st[++top] = a + b; break;
            case '-': st[++top] = a - b; break;
            case '*': st[++top] = a * b; break;
            case '/': if (b == 0) { printf("\n  !! division by zero\n");
                                    err = 1; st[++top] = 0; }
                      else st[++top] = a / b;
                      break;
            case '%': if (b == 0) { printf("\n  !! modulo by zero\n");
                                    err = 1; st[++top] = 0; }
                      else st[++top] = a % b;
                      break;
            default: break;
            }
            printf("\n  '%c' is an operator -> pop %d and %d, push (%d %c %d) = %d\n",
                   c, b, a, a, c, b, st[top]);
        } else {
            continue;
        }
        draw_stack_int(st, top, MAX_LINE, "OPERAND STACK");
        ui_step();
        if (err) break;
    }
    if (!err && top == 0) printf("\n  RESULT = %d\n", st[0]);
    else if (!err)        printf("\n  !! malformed expression - %d values left "
                                 "on the stack\n", top + 1);
    ui_complexity("Postfix evaluation", "O(n)", "O(n)", "O(n)",
                  "O(n) operand stack", "No precedence rules needed");
    ui_pause(NULL);
}

/* ======================================== APPLICATION : TOWER OF HANOI */
#define HANOI_MAX 6
static int  peg[3][HANOI_MAX];
static int  pcount[3];
static int  hanoi_n, hanoi_moves;

static void hanoi_draw(void)
{
    int w = 2 * hanoi_n + 1, lvl, p, i, d, pad;
    printf("\n");
    for (lvl = hanoi_n - 1; lvl >= 0; lvl--) {
        printf("   ");
        for (p = 0; p < 3; p++) {
            if (lvl < pcount[p]) {
                d   = peg[p][lvl];
                pad = (w - (2 * d - 1)) / 2;
                for (i = 0; i < pad; i++) putchar(' ');
                for (i = 0; i < 2 * d - 1; i++) putchar('=');
                for (i = 0; i < pad; i++) putchar(' ');
            } else {
                pad = w / 2;
                for (i = 0; i < pad; i++) putchar(' ');
                putchar('|');
                for (i = 0; i < pad; i++) putchar(' ');
            }
            printf("   ");
        }
        printf("\n");
    }
    printf("   ");
    for (p = 0; p < 3; p++) { for (i = 0; i < w; i++) putchar('-'); printf("   "); }
    printf("\n   ");
    for (p = 0; p < 3; p++) {
        int lead = (w - 1) / 2;
        for (i = 0; i < lead; i++) putchar(' ');
        putchar('A' + p);
        for (i = 0; i < w - lead - 1; i++) putchar(' ');
        printf("   ");
    }
    printf("\n");
}

static void hanoi_move(int from, int to)
{
    int d = peg[from][--pcount[from]];
    peg[to][pcount[to]++] = d;
    hanoi_moves++;
    printf("\n  Move %2d : disk %d  from peg %c to peg %c\n",
           hanoi_moves, d, 'A' + from, 'A' + to);
    hanoi_draw();
    ui_step();
}

static void hanoi_solve(int n, int from, int to, int aux)
{
    if (n == 0) return;
    hanoi_solve(n - 1, from, aux, to);
    hanoi_move(from, to);
    hanoi_solve(n - 1, aux, to, from);
}

static void hanoi_demo(void)
{
    int i;
    ui_header("APPLICATION : TOWER OF HANOI (the recursion / call stack)");
    hanoi_n = ui_read_int("\n  Number of disks (1-6): ", 1, HANOI_MAX);
    pcount[0] = pcount[1] = pcount[2] = 0;
    for (i = hanoi_n; i >= 1; i--) peg[0][pcount[0]++] = i;
    hanoi_moves = 0;
    printf("\n  Start position (all disks on peg A):\n");
    hanoi_draw();
    ui_step();
    hanoi_solve(hanoi_n, 0, 2, 1);
    printf("\n  Solved in %d moves. Minimum possible = 2^%d - 1 = %d.\n",
           hanoi_moves, hanoi_n, (1 << hanoi_n) - 1);
    ui_complexity("Tower of Hanoi", "O(2^n)", "O(2^n)", "O(2^n)",
                  "O(n) recursion stack depth",
                  "Each call pushes a frame onto the system stack");
    ui_pause(NULL);
}

/* ================================================================== menu */
void stack_menu(void)
{
    static const char *const items[] = {
        "Stack using an array",
        "Stack using a linked list",
        "Application : Balancing symbols",
        "Application : Infix to Postfix conversion",
        "Application : Postfix evaluation",
        "Application : Tower of Hanoi (recursion)"
    };
    int choice;
    for (;;) {
        choice = ui_menu("STACK  (Unit 3)", items, 6, "Back to main menu");
        if (choice == 0 || g_eof) return;
        switch (choice) {
        case 1: array_stack_menu();  break;
        case 2: linked_stack_menu(); break;
        case 3: balancing_demo();    break;
        case 4: infix_demo();        break;
        case 5: postfix_demo();      break;
        case 6: hanoi_demo();        break;
        default: break;
        }
    }
}
