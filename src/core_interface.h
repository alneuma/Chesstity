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
#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

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

/********************************************************************
 * create_game: Creates a Game object, which is a linked list, with *
 *              variables of type Game_state as nodes.              *
 *              Returns NULL on failure.                            *
 ********************************************************************/
Game create_game(void);

/********************************************************************
 * duplicate_came: Returns a copy of game.                          *
 *                 Returns NULL on failure.                         *
 ********************************************************************/
Game duplicate_game(Game game);

/********************************************************************
 * destroy_game: Destroys a Game object freeing all the used        *
 *               memory.                                            *   
 ********************************************************************/
void destroy_game(Game game);

/********************************************************************
 * move: returns different codes based
 ********************************************************************/
int move(Game game, Move move);

/********************************************************************
 * player_to_move: Returns the color of the player to move.         *
 *                 Color can be NONE, WHITE or BLACK                *
 ********************************************************************/
Color player_to_move(Game game);

/********************************************************************
 * pawn_upgradable: Checks if there is an upgradable pawn for the   *
 *                  passive player (i.e. the one who just moved).   *
 *                  Should be used to check if a graphical          *
 *                  front-end should be called to pass it's input   *
 *                  to upgrade_pawn().                              *
 ********************************************************************/
Square pawn_upgradable(Game game);

/********************************************************************
 * upgrade_pawn: Replaces the (only) upgradable pawn, by the        *
 *               according piece.                                   *
 ********************************************************************/
bool upgrade_pawn(Game game, Letter_piece piece);

/********************************************************************
 * victory_state: returns different codes based
 ********************************************************************/
int victory_state(Game game);

/********************************************************************
 * current_board: Returns a pointer to a staticly stored            *
 *                64-letter-string, representing the actual board.  *
 *                To be passed to graphic-output.                   *
 ********************************************************************/
const Letter_piece *current_board(Game game);

#endif
