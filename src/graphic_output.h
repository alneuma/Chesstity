#ifndef GRAPHIC_OUTPUT_H
#define GRAPHIC_OUTPUT_H

#include "core_functions.h"

/********************************************************************
 * piece_to_letter: Returns the letter associated with a piece.     *
 ********************************************************************/
char piece_to_letter(Piece_i *piece);

/********************************************************************
 * print_board: Prints game_state->board as an ascii chess-board.   *
 ********************************************************************/
void print_board(Game_state *game_state);

#endif
