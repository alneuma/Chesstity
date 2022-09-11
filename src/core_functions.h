#ifndef CORE_FUNCTIONS_H
#define CORE_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>

#define BOARD_ROWS 8
#define BOARD_COLUMNS 8

typedef enum color {
    NONE, WHITE, BLACK,
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
    Square king_black;
    bool possible_moves[BOARD_ROWS][BOARD_COLUMNS][BOARD_ROWS][BOARD_COLUMNS];
    int possible_moves_number;
    struct game_state *previous_game_state;
} Game_state;

/********************************************************************
 * active_player: Returns the color of the active player.           *
 ********************************************************************/
Color players_turn(Game_state *game_state);

/********************************************************************
 * king_square: Returns a pointer to the square of players's king   *
 *              at a given Game_state.                              *
 ********************************************************************/
Square *king_square(Game_state *game_state, Color player);

/********************************************************************
 * update_possible_moves: Looks at certain parameters of a          *
 *                        Game_state structure and writes it's      *
 *                        possible moves.                           *
 ********************************************************************/
void update_possible_moves_game(Game_state *game_state);

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
void write_possible_moves_square(Game_state *game_state, Square square);

/********************************************************************
 * write_pawn_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumption.            *
 ********************************************************************/
void write_pawn_possible_moves(Game_state *state, Square *square);

/********************************************************************
 * write_knight_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
void write_knight_possible_moves(Game_state *state, Square *square);

/********************************************************************
 * write_bishop_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
void write_bishop_possible_moves(Game_state *state, Square *square);

/********************************************************************
 * write_rook_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumptions.           *
 ********************************************************************/
void write_rook_possible_moves(Game_state *state, Square *square);

/********************************************************************
 * write_king_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumptions.           *
 ********************************************************************/
void write_king_possible_moves(Game_state *state, Square *square);

/********************************************************************
 * in_check_after_move: Checks if the moving player at a given game *
 *                      state would be in check after a potential   *
 *                      move.                                       *
 ********************************************************************/
bool in_check_after_move(Game_state *state, Move move);

/********************************************************************
 * is_attacked: To see if rochade is possible, it is necessary to   *
 *           check if unoccupied squares are attacked by a specific *
 *           player. Thats why the color of the defending player    *
 *           needs to be specified. This also could prove useful    *
 *           for the AI to plan moves.                              *
 ********************************************************************/
bool is_attacked(Game_state *game_state, Square square, Color attacking_player);

#endif
