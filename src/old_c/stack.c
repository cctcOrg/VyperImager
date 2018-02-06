#include <stdlib.h>
#include "stack.h"

page_stack *new_stack() {
    page_stack *stack = malloc(sizeof(page_stack));
    stack->tos = -1;
    return stack;
}

void push_stack(page_stack *p, int page) {
    p->pages[++p->tos] = page;
}

int pop_stack(page_stack *p) {
    return p->pages[p->tos--];
}

int peek_stack(page_stack *p) {
    return p->pages[p->tos];
}

void clear_stack(page_stack *p)
{
    while (p->tos != -1)
    {
        pop_stack(p);
    }
}
