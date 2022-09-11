#include "core_functions.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/*
typedef enum color {
    EMPTY, WHITE, BLACK,
} Color;

typedef enum piece_kind {
    EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
} Piece_kind;

typedef struct piece {
    Color color;
    Piece_kind kind;
} Piece;

typedef struct square {
    int row;
    int column;
} Square;

typedef struct move {
    Square from;
    Square to;
} Move;

typedef struct game_state {
    int move_number;
    int uneventful_moves_number;
    int repeated_boards_number;
    bool rochade_small_legal_white;
    bool rochade_big_legal_white;
    bool rochade_small_legal_black;
    bool rochade_big_legal_black;
    Move last_move;
    Piece board[BOARD_ROWS][BOARD_COLUMNS];
    Square king_white;
    Square king black;
    bool possible_moves[BOARD_ROWS][BOARD_COLUMNS][BOARD_ROWS][BOARD_COLUMNS];
    int possible_moves_number;
    struct game_state *previous_game_state;
} Game_state;
*/

/********************************************************************
 * active_player: Returns the color of the active player.           *
 ********************************************************************/
Color players_turn(Game_state *game_state)
{
    return ((game_state->move_number - 1) % 2) + 1;
}

/********************************************************************
 * king_square: Returns a pointer to the square of players's king   *
 *              at a given Game_state.                              *
 ********************************************************************/
Square *king_square(Game_state *game_state, Color player)
{
    if (WHITE == player)
        return &game_state->king_white;
    else if (BLACK == player)
        return &game_state->king_black;
    else
    {
        printf("error: %s: invalid player color\n", __func__);
        exit(EXIT_SUCCESS);
    }
}

/********************************************************************
 * update_possible_moves: Looks at certain parameters of a          *
 *                        Game_state structure and writes it's      *
 *                        possible moves.                           *
 ********************************************************************/
/*
void update_possible_moves_game(Game_state *game_state)
{
    Color active_player = active_player(game_state);
    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; < BOARD_COLUMNS; ++j)
        {
            if (game_state->board[i][j].color == active_player)
            {
                write_possible_moves_square(game_state, (struct square) { .row = i, .column = j });
            }
        }
    }
}
*/

/********************************************************************
 * write_possible_moves_square: writes game_state->possible_moves   *
 *                              and adds to                         *
 *                              game_state->possible_moves_number   *
 *                              for the piece on one square.        *
 *                                                                  *
 *                              IMPORTANT:                          *
 *                              Assumes that all entries of         *
 *                              game_state->possible moves are set  *
 *                              to false.                           *
 ********************************************************************/
/*
void write_possible_moves_square(Game_state *game_state, Square square)
{
    Color active_player = players_turn(game_state->move_number);
    if (game_state->board[square.row][square.column].color != active_player)
        return;
    switch (game_state->board[square.row][square.column].kind)
    {
        case PAWN:      write_pawn_possible_moves(game_state, &square);
                        break;
        case KNIGHT:    write_knight_possible_moves(game_state, &square);
                        break;
        case BISHOP:    write_bishop_possible_moves(game_state, &square);
                        break;
        case ROOK:      write_rook_possible_moves(game_state, &square);
                        break;
        case QUEEN:     write_bishop_possible_moves(game_state, &square);
                        write_rook_possible_moves(game_state, &square);
                        break;
        case KING:      write_king_possible_moves(game_state, &square);
                        break;
        default:        printf("error: %s: invalid kind of piece\n", __func__);
                        exit(EXIT_SUCCESS);
    }
}
*/

/********************************************************************
 * write_pawn_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumption.            *
 ********************************************************************/
//write_pawn_possible_moves(Game_state *state, Square *square)

/********************************************************************
 * write_knight_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
/*
void write_knight_possible_moves(Game_state *state, Square *square)
{
    int row_modifier[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int column_modifier[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
*/

/********************************************************************
 * write_bishop_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
//write_bishop_possible_moves(Game_state *state, Square *square)

/********************************************************************
 * write_rook_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumptions.           *
 ********************************************************************/
void write_rook_possible_moves(Game_state *state, Square *square)
{
    Color active_player = players_turn(state);

    // left
    int row = square->row;
    int column;
    for (column = square->column - 1; column >= 0; --column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if (active_player != state->board[row][column].color)
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // right
    for (column = square->column + 1; column < BOARD_COLUMNS; ++column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if (active_player != state->board[row][column].color)
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // above
    column = square->column;
    for (row = square->row + 1; row < BOARD_ROWS; ++row)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if (active_player != state->board[row][column].color)
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // below
    column = square->column;
    for (row = square->row - 1; row >= 0; --row)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if (active_player != state->board[row][column].color)
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }
}

/********************************************************************
 * write_king_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumptions.           *
 ********************************************************************/
/*
void write_king_possible_moves(Game_state *state, Square *square)
{
    Color active_player = active_player(state->move_number);
    int row, column;
    for (row = square->row - 1; row <= square->row + 1; ++row)
    {
        for (column = square-> column - 1; column <= square->column + 1; ++column)
        {
            if ((active_player == state->board[row][column].color)
             || (row < 0 || row >= BOARD_ROWS)
             || (row == square->row && column == square->column)
             || (column < 0 || column >= BOARD_COLUMNS))
                continue;
            else if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})
            {
                state->possible_moves[square->row][square->column][row][column] = true;
                ++state->possible_moves_number;
            }
        }
    }
}
*/

/********************************************************************
 * in_check_after_move: Checks if the moving player at a given game *
 *                      state would be in check after a potential   *
 *                      move.                                       *
 ********************************************************************/
bool in_check_after_move(Game_state *state, Move move)
{
    Color defending_player = players_turn(state);
    Color attacking_player = (WHITE == defending_player) ? BLACK : WHITE;

    Game_state new_state = *state;
    new_state.board[move.to.row][move.to.column] = new_state.board[move.from.row][move.from.column];
    new_state.board[move.from.row][move.from.column] = (Piece) {NONE, EMPTY};

    if (KING == new_state.board[move.to.row][move.to.column].kind)
    {
        return is_attacked(&new_state, move.to, attacking_player);
    }
    return is_attacked(&new_state, *king_square(&new_state, defending_player), attacking_player);
}

/********************************************************************
 * is_attacked: To see if rochade is possible, it is necessary to   *
 *           check if unoccupied squares are attacked by a specific *
 *           player. Thats why the color of the defending player    *
 *           needs to be specified. This also could prove useful    *
 *           for the AI to plan moves.                              *
 ********************************************************************/
bool is_attacked(Game_state *game_state, Square square, Color attacking_player)
{
    /* check left */
    int row = square.row;
    int column;
    for (column = square.column - 1; column >= 0; --column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((ROOK == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.column - 1 == column))))
                return true;
            else
                break;
        }
    }

    /* check right */
    for (column = square.column + 1; column < BOARD_COLUMNS; ++column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((ROOK == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.column + 1 == column))))
                return true;
            else
                break;
        }
    }
    
    /* check above */
    column = square.column;
    for (row = square.row + 1; row < BOARD_ROWS; ++row)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((ROOK == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row + 1 == row))))
                return true;
            else
                break;
        }
    }

    /* check below */
    for (row = square.row - 1; row < BOARD_ROWS; --row)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((ROOK == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row - 1 == row))))
                return true;
            else
                break;
        }
    }

    /* check upper left */
    for (row = square.row + 1, column = square.column - 1;
         row < BOARD_ROWS && column >= 0; ++row, --column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((BISHOP == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row + 1 == row)
                                                               && (square.column - 1 == column))))
                return true;
            else
                break;
        }
    }

    /* check upper right */
    for (row = square.row + 1, column = square.column + 1;
         row < BOARD_ROWS && column < BOARD_COLUMNS; ++row, ++column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((BISHOP == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row + 1 == row)
                                                               && (square.column + 1 == column))))
                return true;
            else
                break;
        }
    }

    /* check lower left */
    for (row = square.row - 1, column = square.column - 1;
         row >= 0 && column >= 0; --row, --column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((BISHOP == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row - 1 == row)
                                                               && (square.column - 1 == column))))
                return true;
            else
                break;
        }
    }

    /* check lower right */
    for (row = square.row - 1, column = square.column + 1;
         row >= 0 && column < BOARD_COLUMNS; --row, ++column)
    {
        if (EMPTY != game_state->board[row][column].kind)
        {
            if ((game_state->board[row][column].color == attacking_player)
             &&
               ((BISHOP == game_state->board[row][column].kind)
             || (QUEEN == game_state->board[row][column].kind)
             || ((KING == game_state->board[row][column].kind) && (square.row - 1 == row)
                                                               && (square.column + 1 == column))))
                return true;
            else
                break;
        }
    }

    /* check knights */
    int row_modifier[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int column_modifier[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
    for (int i = 0; i < 8; ++i)
    {
        if ((square.row + row_modifier[i] < 0)
         || (square.row + row_modifier[i] >= BOARD_ROWS)
         || (square.column + column_modifier[i] < 0)
         || (square.column + column_modifier[i] >= BOARD_COLUMNS))
            continue;
        else if
            ((KNIGHT == game_state->board[square.row + row_modifier[i]][square.column + column_modifier[i]].kind)
          &&
             (attacking_player == game_state->board[square.row + row_modifier[i]][square.column + column_modifier[i]].color))
            return true;
    }

    /* check pawn */
    int pawn_row_modifier = (WHITE == attacking_player) ? -1 : 1;
    if (square.row + pawn_row_modifier >= 0 || square.row + pawn_row_modifier < BOARD_ROWS)
    {
        if ((square.column - 1 >= 0)
         && (PAWN == game_state->board[square.row + pawn_row_modifier][square.column - 1].kind)
         && (attacking_player == game_state->board[square.row + pawn_row_modifier][square.column - 1].color))
            return true;

        if ((square.column + 1 >= 0)
         && (PAWN == game_state->board[square.row + pawn_row_modifier][square.column + 1].kind)
         && (attacking_player == game_state->board[square.row + pawn_row_modifier][square.column + 1].color))
            return true;
    }

    /* check en passant */
    if
        (((game_state->last_move.from.row + ((WHITE == attacking_player) ? 2 : -2)) == game_state->last_move.to.row)
      &&
          (PAWN == game_state->board[square.row][square.column].kind)
      &&
          (((WHITE == attacking_player) ? BLACK : WHITE) == game_state->board[square.row][square.column].color)
      &&
          (((PAWN == game_state->board[square.row][square.column - 1].kind)
        &&  (attacking_player == game_state->board[square.row][square.column - 1].color))
        ||
          ((PAWN == game_state->board[square.row][square.column + 1].kind)
        && (attacking_player == game_state->board[square.row][square.column + 1].color))))
        return true;

    return false;
}
