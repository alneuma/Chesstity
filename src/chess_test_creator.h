#ifndef CHESS_TEST_CREATION_H
#define CHESS_TEST_CREATION_H

#include "core_functions.h"
#include "core_interface.h"

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square square);

/********************************************************************
 * board_string: Retruns a pointer to a string displaying the       *
 *               current board state in the same format, the        *
 *               board_from_string() function takes as input.       *
 ********************************************************************/
char *board_string(Game_state *state);

/********************************************************************
 * set_test_game_state: Takes a Game_state and sets it's values to  *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_game_state.                            *
 ********************************************************************/
void set_test_game_state(Game_state *state);

#endif
