//
// input.h
// handles user-input
// san stands for "standard algebraic notation"
// see: https://en.wikipedia.org/wiki/Algebraic_notation_(chess) for an explanation
//

#ifndef INPUT_H
#define INPUT_H

#include "core_interface.h"
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////
// read_input: reads a line of text from stdin which can be
//             terminated by either '\n' or EOF.
//             Returns a dynamically allocated '\0' terminated string.
//             It is the callers responsibillity to disallocate the
//             memory, when the sting is not needed anymore.
//             Behavior is undefined if string is not '\0'
//             terminated.
/////////////////////////////////////////////////////////////////////
char *read_input(void);

/////////////////////////////////////////////////////////////////////
// remove_whitespace(): removes leading and trailing whitespace
//                      from a '\0' terminated string.
//                      Behavior is undefined if string is not '\0'
//                      terminated.
/////////////////////////////////////////////////////////////////////
void remove_whitespace(char *string);

/////////////////////////////////////////////////////////////////////
// is_valid_san: Checks if the '\0' terminated input String is in
//               the format of standard algebraic notation.
//               Returns true, if so.
//               Input is not allowed to have leading or trailing
//               whitespace.
//               Behavior is undefined if string is not '\0'
//               terminated or has leading or trailing whitespace.
/////////////////////////////////////////////////////////////////////
bool is_valid_san(char *input);

#endif
