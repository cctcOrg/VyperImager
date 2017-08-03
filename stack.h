#ifndef STACK_H
#define STACK_H

#define MAX_PAGES 10

typedef struct page_stack {
    int tos;
    int pages[MAX_PAGES];
} page_stack;

page_stack *new_stack();
void push_stack(page_stack *p, int page);
int pop_stack(page_stack *p);
int peek_stack(page_stack *p);

#endif /* STACK_H */
