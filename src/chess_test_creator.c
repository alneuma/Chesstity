#include "core_functions.h"
#include "core_interface.h"
#include "chess_test_creator.h"
#include "graphic_output.h"
#include <stdlib.h>
#include <stdbool.h>

enum letter_piece_test_i
{
    NONE_EMPTY_test_i = '.',
    WHITE_PAWN_test_i = 'P',
    WHITE_KNIGHT_test_i = 'N',
    WHITE_BISHOP_test_i = 'B',
    WHITE_ROOK_test_i = 'R',
    WHITE_QUEEN_test_i = 'Q',
    WHITE_KING_test_i = 'K', 
    BLACK_PAWN_test_i = 'p',
    BLACK_KNIGHT_test_i = 'n',
    BLACK_BISHOP_test_i = 'b',
    BLACK_ROOK_test_i = 'r',
    BLACK_QUEEN_test_i = 'q',
    BLACK_KING_test_i = 'k', 
};

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square_i square)
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

//    state->last_move = (Move_i) { (Square_i) {0,0}, (Square_i) {0,0} };

    for (int i = 0; i < BOARD_ROWS; i++)
    {
        for (int j = 0; j < BOARD_COLUMNS; j++)
        {
            state->board[i][j] = (Piece_i) {NONE_i,EMPTY};
        }
    }

//    state->board[4][0] = (Piece_i) {WHITE_i,KING};
//    state->king_white = (Square_i) {4,0};
//    state->board[4][BOARD_ROWS-1] = (Piece_i) {BLACK_i,KING};
//    state->king black = (Square_i) {4,BORAD_ROWS-1};

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

/********************************************************************
 * letter_to_piece: Converts an int (Letter_piece) representation   *
 *                  of a piece which is used for writing            *
 *                  chess-debugging-scripts into the internally     *
 *                  used struct (Piece_i) representation of a piece *
 *                  type.                                           *
 ********************************************************************/
static Piece_i letter_to_piece(const Letter_piece_test_i letter)
{
    switch (letter)
    {
        case NONE_EMPTY_test_i:         return (Piece_i) {NONE_i, EMPTY};
        case WHITE_PAWN_test_i:    return (Piece_i) {WHITE_i, PAWN};
        case WHITE_KNIGHT_test_i:  return (Piece_i) {WHITE_i, KNIGHT};
        case WHITE_BISHOP_test_i:  return (Piece_i) {WHITE_i, BISHOP};
        case WHITE_ROOK_test_i:    return (Piece_i) {WHITE_i, ROOK};
        case WHITE_QUEEN_test_i:   return (Piece_i) {WHITE_i, QUEEN};
        case WHITE_KING_test_i:    return (Piece_i) {WHITE_i, KING};
        case BLACK_PAWN_test_i:    return (Piece_i) {BLACK_i, PAWN};
        case BLACK_KNIGHT_test_i:  return (Piece_i) {BLACK_i, KNIGHT};
        case BLACK_BISHOP_test_i:  return (Piece_i) {BLACK_i, BISHOP};
        case BLACK_ROOK_test_i:    return (Piece_i) {BLACK_i, ROOK};
        case BLACK_QUEEN_test_i:   return (Piece_i) {BLACK_i, QUEEN};
        case BLACK_KING_test_i:    return (Piece_i) {BLACK_i, KING};
        default:                printf("error: %s: unknown letter representation of piece; terminating\n", __func__);
                                exit(EXIT_FAILURE);
    }
}

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
void set_board(Game_state *state, const Letter_piece_test_i *board_string)
{
    const char *p = board_string;
    int i = BOARD_ROWS;
    int j;
    while (*p)
    {
        if ((j = (p - board_string) % BOARD_COLUMNS) == 0)
            i--;

        if (WHITE_KING_test_i == *p)
            state->king_white = (Square_i) {i,j};
        else if (BLACK_KING_test_i == *p)
            state->king_black = (Square_i) {i,j};
        
        state->board[i][j] = letter_to_piece(*p++);
    }
}
