/********************************************************************
 * core_interface.h                                                 *
 *                                                                  *
 * The interface for other modules to communicate with the          *
 * core-functionality. Communication happens largely through char   *
 * and string variables. Which are meant to be easily               *
 * understandable by the programmer.                                *
 * empty square:        ' '                                         *
 * white/black pawn:    'P'/'p'                                     *
 * white/black knight:  'N'/'n'                                     *
 * white/black bishop:  'B'/'b'                                     *
 * white/black rook:    'R'/'r'                                     *
 * white/black queen:   'Q'/'q'                                     *
 * white/black king:    'K'/'k'                                     *
 ********************************************************************/
// settings to enable debugging
#define DEBUG

#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "core_functions.h"
#include <stdbool.h>

typedef struct game *Game;

typedef char Letter_piece;

typedef enum color {
    NONE, WHITE, BLACK,
} Color;

typedef struct square {
    int row;
    int column;
} Square;

typedef struct move {
    Square from;
    Square to;
} Move;

#ifdef DEBUG
void set_game_state(Game_state *state, const Letter_piece *board);
enum letter_piece
{
    NONE_EMPTY = '.',
    WHITE_PAWN = 'P',
    WHITE_KNIGHT = 'N',
    WHITE_BISHOP = 'B',
    WHITE_ROOK = 'R',
    WHITE_QUEEN = 'Q',
    WHITE_KING = 'K', 
    BLACK_PAWN = 'p',
    BLACK_KNIGHT = 'n',
    BLACK_BISHOP = 'b',
    BLACK_ROOK = 'r',
    BLACK_QUEEN = 'q',
    BLACK_KING = 'k', 
};
#endif

/********************************************************************
 * san_to_move: converts a null-terminated string in SAN (standard
 *              algebraic notation) into a move.
 *              Returns
 *              { {-1,-1}, {0,0} } if move is ambigious or
 *              { {-1,-1}, {1,1} } if move is illegal
 *              Does not error-check. Correct format is assumed.
 *              !!!! draw claim/proposal and pawn creation not included
 *              (also not in input)
 ********************************************************************/
Move san_to_move(const char *san, const Game game);

/********************************************************************
 * create_game: Creates a Game object.                              *
 *              Returns NULL on failure.                            *
 ********************************************************************/
Game create_game(void);

/********************************************************************
 * destroy_game: Destroys a Game object                             *
 ********************************************************************/
void destroy_game(Game game);

/********************************************************************
 * duplicate_came: Returns a copy of game.                          *
 *                 Returns NULL on failure.                         *
 ********************************************************************/
Game duplicate_game(const Game game);

/********************************************************************
 * move_piece: returns different codes based                        *
 *              true = normal move; success                         *
 *             false = move illegal                                 *
 ********************************************************************/
bool move_piece(Game game, const Move move);

/********************************************************************
 * claim_remis_move: Returns true if move with remis claim will     *
 *                   lead to remis.                                 *
 *                   Otherwise returns false.                       *
 ********************************************************************/
bool claim_remis_move(const Game game, const Move move);

/********************************************************************
 * player_to_move: Returns the color of the player to move.         *
 *                 Color can be NONE, WHITE or BLACK                *
 ********************************************************************/
Color player_to_move(const Game game);

/********************************************************************
 * pawn_upgradable: Checks if there is an upgradable pawn for the   *
 *                  passive player (i.e. the one who just moved).   *
 *                  Should be used to check if a graphical          *
 *                  front-end should be called to pass it's input   *
 *                  to upgrade_pawn().                              *
 ********************************************************************/
Square pawn_upgradable(const Game game);

/********************************************************************
 * upgrade_pawn: Replaces the (only) upgradable pawn, by the        *
 *               according piece.                                   *
 ********************************************************************/
void upgrade_pawn(Game game, Letter_piece piece);

/********************************************************************
 * victory_state: returns different codes based
 ********************************************************************/
int victory_state(const Game game);

/********************************************************************
 * current_board: Returns a pointer to a staticly stored            *
 *                64-letter-string, representing the actual board.  *
 *                To be passed to graphic-output.                   *
 ********************************************************************/
const Letter_piece *current_board(Game game);

#endif
