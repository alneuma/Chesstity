#ifndef CHESS_TEST_CREATION_H
#define CHESS_TEST_CREATION_H

#include "core_functions.h"

typedef char Letter_piece;
enum letter_piece
{
    LET_EMPTY = '.',
    LET_WHITE_PAWN = 'P',
    LET_WHITE_KNIGHT = 'N',
    LET_WHITE_BISHOP = 'B',
    LET_WHITE_ROOK = 'R',
    LET_WHITE_QUEEN = 'Q',
    LET_WHITE_KING = 'K', 
    LET_BLACK_PAWN = 'p',
    LET_BLACK_KNIGHT = 'n',
    LET_BLACK_BISHOP = 'b',
    LET_BLACK_ROOK = 'r',
    LET_BLACK_QUEEN = 'q',
    LET_BLACK_KING = 'k', 
};


/********************************************************************
 * board_from_string: Writes the board_state represented by         *
 *                    board_string into state->board.               *
 *                    board_string should be of the following       *
 *                    format:                                       *
 *                    Letter_piece board_string[] = "rnbqkbnr"      *
 *                                                  "pppppppp"      *
 *                                                  "........"      *
 *                                                  "........"      *
 *                                                  "........"      *
 *                                                  "........"      *
 *                                                  "PPPPPPPP"      *
 *                                                  "RNBQKBNR";     *
 *                    Here uppper case letters represent black      *
 *                    pieces, lower-case letters represent white    *
 *                    pieces and '*' character represent empty      *
 *                    squares.                                      *
 *                    Can be adjusted in chess_test_creater.h       *
 *                    (typedef Letter_piece).                       *
 ********************************************************************/
void board_from_string(Game_state *state, Letter_piece *board_string);

/********************************************************************
 * letter_to_piece: Converts an int (Letter_piece) representation   *
 *                  of a piece which is used for writing            *
 *                  chess-debugging-scripts into the internally     *
 *                  used struct (Piece) representation of a piece   *
 *                  type.                                           *
 ********************************************************************/
Piece letter_to_piece(Letter_piece letter);

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square square);

/********************************************************************
 * set_game_state: Takes a Game_state and sets it's values to       *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_game_state.                            *
 ********************************************************************/
void set_game_state(Game_state *state);

#endif
