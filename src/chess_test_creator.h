#ifndef CHESS_TEST_CREATION_H
#define CHESS_TEST_CREATION_H

#include "core_functions.h"
#include "core_interface.h"

// this is similar to what is declare in core_interface.c
// the suffic _test_i stands for "test internal"
// also the function letter_piece_is similar to the same named static function in core_interface.c
// this is done for encapsulations reasons and because chess_test_creator is not really a part of the program

typedef char Letter_piece_test_i;

/********************************************************************
 * access_state: Returns a pointer to the current_state of a Game.  *
 ********************************************************************/
Game_state *access_state(Game game);

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square_i square);

/********************************************************************
 * board_string: Retruns a pointer to a string displaying the       *
 *               current board state in the same format, the        *
 *               board_from_string() function takes as input.       *
 ********************************************************************/
char *board_string(Game_state *state);

/********************************************************************
 * set_test_game_state: Takes a Game_state and sets it's values to  *
 *                      default testing value.                      *
 *                      Ignores last_move, king_white, king_black   *
 *                      and previous_game_state.                    *
 ********************************************************************/
void set_test_game_state(Game_state *state);

/********************************************************************
 * set_board: Writes the board_state represented by                 *
 *            board_string into state->board.                       *
 *            board_string should be of the following               *
 *            format:                                               *
 *            Letter_piece board_string[] = "rnbqkbnr"              *
 *                                          "pppppppp"              *
 *                                          "........"              *
 *                                          "........"              *
 *                                          "........"              *
 *                                          "........"              *
 *                                          "PPPPPPPP"              *
 *                                          "RNBQKBNR";             *
 *            Here uppper case letters represent black              *
 *            pieces, lower-case letters represent white            *
 *            pieces and '*' character represent empty              *
 *            squares.                                              *
 *            Can be adjusted in chess_test_creater.h               *
 *            (typedef Letter_piece).                               *
 *            Like board_from_string (core_interface.c) but public. *
 ********************************************************************/
void set_board(Game_state *state, const Letter_piece_test_i *board_string);

#endif
