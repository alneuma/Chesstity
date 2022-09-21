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
    int uneventful_moves;
    int board_occurences;
    bool castle_kngsde_legal_white;
    bool castle_qensde_legal_white;
    bool castle_kngsde_legal_black;
    bool castle_qensde_legal_black;
    bool pawn_upgradable;
    Move last_move;
    Piece board[BOARD_ROWS][BOARD_COLUMNS];
    Square king_white;
    Square king_black;
    bool possible_moves[BOARD_ROWS][BOARD_COLUMNS][BOARD_ROWS][BOARD_COLUMNS];
    int possible_moves_number;
    struct game_state *previous_game_state;
} Game_state;

/********************************************************************
 * compare_boards: Compares two boards are the same, i.e. if the    *
 *                 same pieces occupy the same positions.           *
 *                 Other possible Game_state qualities are not      *
 *                 checked.                                         *
 ********************************************************************/
bool compare_boards(Piece board_1[BOARD_ROWS][BOARD_COLUMNS], Piece board_2[BOARD_ROWS][BOARD_COLUMNS]);

/********************************************************************
 * board_repeated: Counts the number of times the actual board-     *
 *                 state occured before in the game.                *
 *                 Is necessary to determine if draw can be         *
 *                 claimed.                                         *
 ********************************************************************/
int board_repeated(Game_state *state);

/********************************************************************
 * apply_move: Returns a pointer to a dynamically allocated         *
 *             Game_state which represents the state of the game    *
 *             after move was performed in state.                   *
 *             Returns NULL if memoryallocation fails.              *
 *             Assumes that move is legal.                          *
 ********************************************************************/
Game_state *apply_move(Game_state *state, Move move);

/********************************************************************
 * player_active: Returns the color of the active player.           *
 ********************************************************************/
Color player_active(Game_state *state);

/********************************************************************
 * player_passive: Returns the color of the active player.          *
 ********************************************************************/
Color player_passive(Game_state *state);

/********************************************************************
 * king_square: Returns a pointer to the square of players's king   *
 *              at a given Game_state.                              *
 ********************************************************************/
Square *king_square(Game_state *game_state, Color player);

/********************************************************************
 * update_possible_moves_game: Looks at certain parameters of a     *
 *                             Game_state structure and writes it's *
 *                             possible moves.                      *
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
 * is_attacked_by: To see if rochade is possible, it is necessary   *
 *                 to check if unoccupied squares are attacked by a *
 *                 specific player. Thats why the color of the      *
 *                 defending player needs to be specified. This     *
 *                 also could prove useful for the AI to plan       *
 *                 moves.                                           *
 ********************************************************************/
bool is_attacked_by(Game_state *game_state, Square square, Color attacking_player);

#endif
