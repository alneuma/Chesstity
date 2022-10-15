/********************************************************************
 * ds_lib
 * provides the following data-structures:
 * 1. int stack
 ********************************************************************/
#ifndef DS_LIB_H
#define DS_LIB_H

#include <stdbool.h>

typedef struct stack_int *Stack_int;

Stack_int stack_int_create(void);
void stack_int_destory(Stack_int stack);
void stack_int_make_empty(Stack_int stack);
bool stack_int_push(Stack_int stack, int n);
bool stack_int_pop(Stack_int stack, int *n);
bool stack_int_is_full(Stack_int stack);
bool stack_int_is_empty(Stack_int stack); 
bool stack_int_peek(Stack_int stack, int *n); 

#endif
