#ifndef MEM_UTILITIES_H
#define MEM_UTILITIES_H

#include <stdlib.h>
#include <stdio.h>

/********************************************************************
 * MEM_TEST: To be called after dynamic memory allocation to ptr.
 *           Aborts program if allocation failed.
 ********************************************************************/
#define MEM_TEST(ptr) \
{ \
    if (NULL == (ptr)) \
    { \
        fprintf(stderr, "error: %s: memory-allocation failed; aborting\n", __func__); \
        exit(EXIT_FAILURE); \
    } \
}

#endif
