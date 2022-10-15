#ifndef MEM_UTILITIES_H
#define MEM_UTILITIES_H

/********************************************************************
 * MEM_TEST: To be called after dynamic memory allocation to ptr.
 *           Aborts program if allocation failed.
 ********************************************************************/
#define MEM_TEST(ptr) \
{ \
    if (NULL == (ptr)) \
    { \
        printf("error: %s: memory-allocation failed; aborting\n", __func__); \
        exit(EXIT_FAILURE); \
    } \
}

#endif
