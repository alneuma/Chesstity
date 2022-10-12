#include "ds_lib.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct stack_int_node {
    int value;
    struct stack_int_node *next;
} Stack_int_node;

#define MEM_FAILURE \
{ \
    printf("error: %s: memory-allocation failed; aborting\n", __func__); \
    exit(EXIT_FAILURE); \
}

bool stack_int_push(Stack_int stack, const int n)
{
    Stack_int_node *new_node = malloc(sizeof(*new_node));
    if (NULL == new_node)
        MEM_FAILURE;

    new_node->value = n;
    new_node->next = stack->top;
    stack->top = new_node;

    return true;
}

void stack_int_make_empty(Stack_int stack);
{
    Stack_int_node *p = stack->top;
    Stack_int_node *temp = NULL;
    while (NULL != p)
    {
        temp = p;
        p = p->next;
        free(temp);
    }
}

bool stack_int_pop(Stack_int stack, int *n)
{
    if (stack_int_is_empty(stack))
    {
        return false;
    }

    *n = stack->top->value;

    Stack_int_node *p = stack->top;
    stack->top = p->next;
    free(p);

    return true;
}

bool stack_int_is_full(const Stack_int stack)
{
    return false;
}

bool stack_int_is_empty(const Stack_int stack)
{
    return NULL == stack->top;
}

bool stack_int_peek(const Stack_int stack, int *n)
{
    if (stack_int_is_empty(stack))
    {
        return false;
    }

    *n = stack->top->value;
    return true;
}

/*
Stack_int stack_int_create(void)
{
    Stack_int new_stack = malloc(sizeof(new_stack));
    if (NULL == new_stack)
        MEM_FAILURE;

    new_stack->top = NULL;

    return new_stack;
}

void stack_int_destory(Stack_int stack)
{
    Stack_int_node *p = stack->top;
    Stack_int_node *temp = NULL;
    while (NULL != p)
    {
        temp = p;
        p = p->next;
        free(temp);
    }
    free(stack);
}
*/
