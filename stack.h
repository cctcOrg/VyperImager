#ifndef STACK_H
#define STACK_H

#define MAX_PAGES 10

typedef struct page_stack {
    int tos;
    int pages[MAX_PAGES];
} page_stack;

/* Creates a newly allocated stack */
page_stack *new_stack();

/* Put a page on the stack */
void push_stack(page_stack *p, int page);

/* Pop a page off the given stack and return its value */
int pop_stack(page_stack *p);

/* Return the value on the top of the stack */
int peek_stack(page_stack *p);

#endif /* STACK_H */
