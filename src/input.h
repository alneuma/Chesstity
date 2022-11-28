/********************************************************************
 * input.h
 * handles user-input
 ********************************************************************/

#ifndef INPUT_H
#define INPUT_H

#include "core_interface.h"

void read_command(char *input, Game game);
void read_move(char *input, Game game);
void read_input(Game game);

#endif
