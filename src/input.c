//
// input.c
// handles user-input
//
// comment out the following line to disable debugging
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

#include "core_interface.h"
#include "graphic_output.h"
#include "mem_utilities.h"
#include <stdio.h>

// initial size of dynamically allocated the string used for saving input (see: read_input())
#define INITIAL_INPUT_SIZE 16

/////////////////////////////////////////////////////////////////////
// read_input: starts with INITIAL_INPUT_SIZE * sizeof(char)
//             memory allocated for saving the input.
//             Whenever the input starts exceeding the allocated
//             space, memory is doubled.
/////////////////////////////////////////////////////////////////////
char *read_input(void)
{
    char *input = malloc(INITIAL_INPUT_SIZE * sizeof(*input));
    MEM_TEST(input);
    int current_size = INITIAL_INPUT_SIZE;
    int count = 0;
    char c;
    while (('\n' != (c = getchar())) && (EOF != c))
    {
        if (count >= current_size - 2)
        {
            input = realloc(input, 2*current_size);
            MEM_TEST(input);
            current_size *= 2;
        }
        *(input + count++) = c;
    }
    // make sure, that ending input with EOF and '\n' has the same effect on the terminal
    if (EOF == c)
        putchar('\n');
    *(input + count) = '\0';
    return input;
}
