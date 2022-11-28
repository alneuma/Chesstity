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

/********************************************************************
 * write_current_board: Writes the board of game into dest in the
 *                      way it should be displayed on the screen.
 *                      orientation, as the name suggests, gives the
 *                      orientation of the displayed board, which
 *                      either should be WHITE or BLACK.
 *                      If valid orientation is provided, it will
 *                      default to WHITE.
 ********************************************************************/
void write_current_board(char *dest, Game game, Color orientation);

/********************************************************************
 * display_game_screen: Displays the games main screen.
 ********************************************************************/
void display_game_screen(game);

#endif
