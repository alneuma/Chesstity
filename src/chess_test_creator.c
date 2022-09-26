#include "core_functions.h"
#include "core_interface.h"
#include "chess_test_creator.h"
#include "graphic_output.h"
#include <stdlib.h>
#include <stdbool.h>

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square square)
{
    static char moves_string[] = "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........";

    for (char *p = moves_string; *p; p++)
        *p = '.';

    moves_string[(BOARD_ROWS-1 - square.row) * 8 + square.column] = '0';
    char *write = moves_string;
    int i, j;
    for (i = BOARD_ROWS-1; i >= 0; i--)
    {
        for (j = 0; j < BOARD_COLUMNS; j++)
        {
            if (true == state->possible_moves[square.row][square.column][i][j])
            {
                *write = '1';
            }
            write++;
        }
    }

    return moves_string;
}

/********************************************************************
 * board_string: Retruns a pointer to a string displaying the       *
 *               current board state in the same format, the        *
 *               board_from_string() function takes as input.       *
 ********************************************************************/
char *board_string(Game_state *state)
{
    static char board_string[] = "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........"
                                 "........";

    for (char *p = board_string; *p; p++)
        *p = '.';

    char *write = board_string;
    int i, j;
    for (i = BOARD_ROWS-1; i >= 0; i--)
    {
        for (j = 0; j < BOARD_COLUMNS; j++)
        {
            if (EMPTY == state->board[i][j].kind)
                *write = '.';
            else
                *write = piece_to_letter(&state->board[i][j]);
            write++;
        }
    }

    return board_string;
}

/********************************************************************
 * set_test_game_state: Takes a Game_state and sets it's values to  *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_game_state.                            *
 ********************************************************************/
void set_test_game_state(Game_state *state)
{
    state->move_number = 1;
    state->uneventful_moves = 0;
    state->board_occurences = 1;
    state->pawn_upgradable = false;

    state->castle_kngsde_legal_white = true;
    state->castle_qensde_legal_white = true;
    state->castle_kngsde_legal_black = true;
    state->castle_qensde_legal_black = true;

//    state->last_move = (Move) { (Square) {0,0}, (Square) {0,0} };

    for (int i = 0; i < BOARD_ROWS; i++)
    {
        for (int j = 0; j < BOARD_COLUMNS; j++)
        {
            state->board[i][j] = (Piece) {NONE,EMPTY};
        }
    }

//    state->board[4][0] = (Piece) {WHITE,KING};
//    state->king_white = (Square) {4,0};
//    state->board[4][BOARD_ROWS-1] = (Piece) {BLACK,KING};
//    state->king black = (Square) {4,BORAD_ROWS-1};

    for (int i = 0; i < BOARD_ROWS; i++)
    {
        for (int j = 0; j < BOARD_COLUMNS; j++)
        {
            for (int k = 0; k < BOARD_ROWS; k++)
            {
                for (int l = 0; l < BOARD_COLUMNS; l++)
                {
                    state->possible_moves[i][j][k][l] = false;
                }
            }
        }
    }

    state->possible_moves_number = 0;

//    struct game_state *previous_game_state;
}
