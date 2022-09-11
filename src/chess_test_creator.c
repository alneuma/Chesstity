#include "core_functions.h"
#include "chess_test_creator.h"
#include <stdlib.h>

/********************************************************************
 * board_from_string: Writes the board_state represented by         *
 *                    board_string into state->board.               *
 *                    board_string should be of the following       *
 *                    format:                                       *
 *                    Letter_piece board_string[] = "rnbqkbnr"      *
 *                                                  "pppppppp"      *
 *                                                  "********"      *
 *                                                  "********"      *
 *                                                  "********"      *
 *                                                  "********"      *
 *                                                  "PPPPPPPP"      *
 *                                                  "RNBQKBNR";     *
 *                    Here uppper case letters represent black      *
 *                    pieces, lower-case letters represent white    *
 *                    pieces and '*' character represent empty      *
 *                    squares.                                      *
 *                    Can be adjusted in chess_test_creater.h       *
 *                    (typedef Letter_piece).                       *
 ********************************************************************/
void board_from_string(Game_state *state, Letter_piece *board_string)
{
    char *p = board_string;
    int i = BOARD_ROWS;
    int j;
    while (*p)
    {
        if ((j = (p - board_string) % BOARD_COLUMNS) == 0)
            --i;
        state->board[i][j] = letter_to_piece(*p++);
    }
}

/********************************************************************
 * letter_to_piece: Converts an int (Letter_piece) representation   *
 *                  of a piece which is used for writing            *
 *                  chess-debugging-scripts into the internally     *
 *                  used struct (Piece) representation of a piece   *
 *                  type.                                           *
 ********************************************************************/
Piece letter_to_piece(Letter_piece letter)
{
    switch (letter)
    {
        case LET_EMPTY:         return (Piece) {NONE, EMPTY};
        case LET_WHITE_PAWN:    return (Piece) {WHITE, PAWN};
        case LET_WHITE_KNIGHT:  return (Piece) {WHITE, KNIGHT};
        case LET_WHITE_BISHOP:  return (Piece) {WHITE, BISHOP};
        case LET_WHITE_ROOK:    return (Piece) {WHITE, ROOK};
        case LET_WHITE_QUEEN:   return (Piece) {WHITE, QUEEN};
        case LET_WHITE_KING:    return (Piece) {WHITE, KING};
        case LET_BLACK_PAWN:    return (Piece) {BLACK, PAWN};
        case LET_BLACK_KNIGHT:  return (Piece) {BLACK, KNIGHT};
        case LET_BLACK_BISHOP:  return (Piece) {BLACK, BISHOP};
        case LET_BLACK_ROOK:    return (Piece) {BLACK, ROOK};
        case LET_BLACK_QUEEN:   return (Piece) {BLACK, QUEEN};
        case LET_BLACK_KING:    return (Piece) {BLACK, KING};
        default:                printf("error: %s: unknown letter representation of piece; terminating\n", __func__);
                                exit(EXIT_SUCCESS);
    }
}

/********************************************************************
 * possible_moves_string: Retruns a pointer to a string displaying  *
 *                        the possible moves of saquare at state.   *
 *                        The string is staticly stored inside the  *
 *                        functions body.                           *
 ********************************************************************/
char *possible_moves_string(Game_state *state, Square square)
{
    static char moves_string[] = "00000000"
                                 "00000000"
                                 "00000000"
                                 "00000000"
                                 "00000000"
                                 "00000000"
                                 "00000000"
                                 "00000000";
    
    char *write = moves_string;
    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; j < BOARD_COLUMNS; ++j)
        {
            if (state->possible_moves[square.row][square.column][i][j])
            {
                *write = '1';
            }
            ++write;
        }
    }

    return &moves_string;
}


/********************************************************************
 * set_game_state: Takes a Game_state and sets it's values to       *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_game_state.                            *
 ********************************************************************/
void set_game_state(Game_state *state)
{
    state->move_number = 1;
    state->uneventful_moves_number = 0;
    state->repeated_boards_number = 0;

    state->rochade_small_legal_white = true;
    state->rochade_big_legal_white = true;
    state->rochade_small_legal_black = true;
    state->rochade_big_legal_black = true;

//    state->last_move = (Move) { (Square) {0,0}, (Square) {0,0} };

    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; j < BOARD_COLUMNS; ++j)
        {
            state->board[i][j] = (Piece) {NONE,EMPTY};
        }
    }

//    state->board[4][0] = (Piece) {WHITE,KING};
//    state->king_white = (Square) {4,0};
//    state->board[4][BOARD_ROWS-1] = (Piece) {BLACK,KING};
//    state->king black = (Square) {4,BORAD_ROWS-1};

    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; j < BOARD_COLUMNS; ++j)
        {
            for (int k = 0; k < BOARD_ROWS; ++k)
            {
                for (int l = 0; l < BOARD_COLUMNS; ++l)
                {
                    state->possible_moves[i][j][k][l] = false;
                }
            }
        }
    }

    state->possible_moves_number = 0;

//    struct game_state *previous_game_state;
}


