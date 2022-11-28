/********************************************************************
 * input.c
 * handles user-input
 ********************************************************************/
// comment out the following line to disable debugging
#define DEBUG
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

#include "core_interface.h"
#include "graphic_output.h"
#include "mem_utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

    




    

/*
#define INITIAL_INPUT_SIZE 16
char *input = NULL;

PRIVATE read_command(char *input, Game game);
PRIVATE read_move(char *input, Game game);
void read_input(Game game)
{
    
}

void read_line(void)
{
    static input_size = 0;
    if (NULL == input)
    {
        input = malloc(INITIAL_INPUT_SIZE * sizeof(*input));
        MEM_TEST(input);
        input_size = INITIAL_INPUT_SIZE;
    }
    int count = 0;
    int c;
    while (('\n' != (c = getchar())) && (EOF != c))
    {
        if (count >= input_size - 2)
        {
            realloc(input, 2*input_size);
            MEM_TEST(input);
            input_size *= 2;
        }
        *(input + count++) = c;
    }
    if (EOF == c)
        putchar('\n');

    *(input + count) = '\0';
}

bool validate_line(char *line)
{
*/
