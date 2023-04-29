// Copyright: (c) 2023, Alrik Neumann
// GNU General Public License v3.0+ (see LICENSE.txt or https://www.gnu.org/licenses/gpl-3.0.txt)

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
