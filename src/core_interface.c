// settings to enable debugging
#define DEBUG
#ifndef DEBUG
#define PRIVATE static
#else
#define PRIVATE
#endif

#include "core_functions.h"
#include "core_interface.h"
#include "san_parsing.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// the starting setup of the board
#define STARTING_BOARD "rnbqkbnr"   \
                       "pppppppp"   \
                       "........"   \
                       "........"   \
                       "........"   \
                       "........"   \
                       "PPPPPPPP"   \
                       "RNBQKBNR"

#ifndef DEBUG
// when DEBUG is defined in core_interface.h letter_piece will be defined there
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

struct game
{
    Game_state *current_state;
};

PRIVATE void board_from_string(Game_state *state, const Letter_piece *board_string);
PRIVATE void set_game_state(Game_state *state, const Letter_piece *board);
PRIVATE Piece_i letter_to_piece(const Letter_piece letter);
PRIVATE Letter_piece piece_to_letter_interf(const Piece_i *piece);

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
Move san_to_move(const char *san, const Game game)
{
}

/********************************************************************
 * create_game: Creates a Game object, which is a linked list, with *
 *              variables of type Game_state as nodes.              *
 ********************************************************************/
Game create_game(void)
{
    Game new_game = malloc(sizeof(new_game));
    if (NULL == new_game)
    {
        printf("error: %s: memory-allocation for new game failed", __func__);
        exit(EXIT_FAILURE);
    }

    Game_state *beg_state = malloc(sizeof(*beg_state));
    if (NULL == beg_state)
    {
        printf("error: %s: memory-allocation for new game failed", __func__);
        exit(EXIT_FAILURE);
    }

    set_game_state(beg_state, STARTING_BOARD);

    new_game->current_state = beg_state;

    return new_game;
}

/********************************************************************
 * destroy_game: Destroys a Game object freeing all the used        *
 *               memory.                                            *   
 ********************************************************************/
void destroy_game(Game game)
{
    Game_state *temp = NULL;

    while (NULL != game->current_state->previous_state)
    {
        temp = game->current_state;
        game->current_state = game->current_state->previous_state;
        free(temp);
    }
    free(game->current_state);
    free(game);
}

/********************************************************************
 * duplicate_game: Returns a copy of game.                          *
 *                 Returns NULL on failure.                         *
 ********************************************************************/
Game duplicate_game(Game original_game)
{
    Game duplicate_game = malloc(sizeof(duplicate_game));
    if (NULL == duplicate_game)
    {
        printf("error: %s: memory-allocation for duplicate game failed", __func__);
        exit(EXIT_FAILURE);
    }

    Game_state *original_state = original_game->current_state;

    Game_state *duplicate_state = malloc(sizeof(*duplicate_state));
    if (NULL == duplicate_state)
    {
        printf("error: %s: memory-allocation for duplicate gamestate failed", __func__);
        exit(EXIT_FAILURE);
    }
    *duplicate_state = *original_state;

    duplicate_game->current_state = duplicate_state;

    while (NULL != original_state->previous_state)
    {
        original_state = original_state->previous_state;

        duplicate_state->previous_state = malloc(sizeof(*duplicate_state->previous_state));
        if (NULL == duplicate_state->previous_state)
        {
            printf("error: %s: memory-allocation for duplicate gamestate failed", __func__);
            exit(EXIT_FAILURE);
        }
        *duplicate_state->previous_state = *original_state;
        duplicate_state = duplicate_state->previous_state;
    }
    duplicate_state->previous_state = NULL;

    return duplicate_game;
}

/********************************************************************
 * move_piece: returns different codes based                        *
 *              true = normal move; success                         *
 *             false = move illegal                                 *
 ********************************************************************/
bool move_piece(Game game, const Move move)
{
    if (game->current_state->possible_moves[move.from.row][move.from.column][move.to.row][move.to.column])
    {
        Game_state *new_state = apply_move(game->current_state,
                                          (Move_i) { (Square_i) {move.from.row, move.from.column}, (Square_i) {move.to.row, move.to.column} });
        if (NULL == new_state)
        {
            printf("error: %s: memory-allocation failed; aborting\n", __func__);
            exit(EXIT_FAILURE);
        }
        game->current_state = new_state;
        return true;
    }

    return false;
}

/********************************************************************
 * claim_remis_move: Returns true if move with remis claim will     *
 *                   lead to remis by threefold-repetition-rule.    *
 *                   Otherwise returns false.                       *
 ********************************************************************/
bool claim_remis_move(Game game, Move move)
{
    Game_state *remis_state = apply_move(game->current_state, (Move_i) {
                                            (Square_i) {move.from.row, move.from.column},
                                            (Square_i) {move.to.row, move.to.column}}); 
    if (NULL == remis_state)
    {
        printf("error: %s: memory-allocation failed; aborting\n", __func__);
        exit(EXIT_FAILURE);
    }

    if (3 <= remis_state->board_occurences)
    {
        free(remis_state);
        return true;
    }

    return false;
    free(remis_state);
}

/********************************************************************
 * player_to_move: Returns the color of the player to move.         *
 *                 Color can be NONE, WHITE or BLACK                *
 ********************************************************************/
Color player_to_move(const Game game)
{
    if (WHITE_i == player_active(game->current_state))
        return WHITE;
    return BLACK;
}

/********************************************************************
 * pawn_upgradable: Checks if there is an upgradable pawn for the   *
 *                  passive player (i.e. the one who just moved).   *
 *                  Should be used to check if a graphical          *
 *                  front-end should be called to pass it's input   *
 *                  to upgrade_pawn().                              *
 *                  Returns (Square) {-1, -1} if no upgradable pawn *
 *                  exists.                                         *
 ********************************************************************/
Square pawn_upgradable(const Game game)
{
    if (game->current_state->pawn_upgradable)
        return (Square) {game->current_state->last_move.to.row, game->current_state->last_move.to.column};
    return (Square) {-1,-1};
}

/********************************************************************
 * upgrade_pawn: Replaces the (only) upgradable pawn, by the        *
 *               according piece.                                   *
 *               No error-checking!                                 *
 ********************************************************************/
void upgrade_pawn(Game game, const Letter_piece piece)
{
    game->current_state->board[game->current_state->last_move.to.row][game->current_state->last_move.to.column]
        = (Piece_i) {letter_to_piece(piece).color, letter_to_piece(piece).kind};
}

/********************************************************************
 * victory_state: returns different codes based
 ********************************************************************/
int victory_state(Game game);

/********************************************************************
 * current_board: Returns a pointer to a staticly stored            *
 *                64-letter-string, representing the actual board.  *
 *                To be passed to graphic-output.                   *
 ********************************************************************/
const Letter_piece *current_board(Game game)
{
    static Letter_piece board_string[] = {NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, 
                                          NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY, NONE_EMPTY};

    for (Letter_piece *p = board_string; *p; p++)
        *p = NONE_EMPTY;

    Letter_piece *write = board_string;
    int i, j;
    for (i = BOARD_ROWS-1; i >= 0; i--)
    {
        for (j = 0; j < BOARD_COLUMNS; j++)
        {
            if (EMPTY != game->current_state->board[i][j].kind)
                *write = piece_to_letter_interf(&game->current_state->board[i][j]);
            write++;
        }
    }

    return board_string;
}


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
PRIVATE void board_from_string(Game_state *state, const Letter_piece *board_string)
{
    const char *p = board_string;
    int i = BOARD_ROWS;
    int j;
    while (*p)
    {
        if ((j = (p - board_string) % BOARD_COLUMNS) == 0)
            i--;

        if (WHITE_KING == *p)
            state->king_white = (Square_i) {i,j};
        else if (BLACK_KING == *p)
            state->king_black = (Square_i) {i,j};
        
        state->board[i][j] = letter_to_piece(*p++);
    }
}

/********************************************************************
 * piece_to_letter_interf: Takes and argument of type Piece_i and returns  *
 *                  the according letter as specified in            *
 *                  enum letter_piece.                              *
 ********************************************************************/
PRIVATE Letter_piece piece_to_letter_interf(const Piece_i *piece)
{
    switch (piece->color)
    {
        case NONE_i:    return NONE_EMPTY;
        case WHITE_i:   switch (piece->kind)
                        {
                            case PAWN:      return WHITE_PAWN;
                            case KNIGHT:    return WHITE_KNIGHT;
                            case BISHOP:    return WHITE_BISHOP;
                            case ROOK:      return WHITE_ROOK;
                            case QUEEN:     return WHITE_QUEEN;
                            case KING:      return WHITE_KING;
                            default:        printf("error: %s: unkown piece; terminating\n", __func__);
                                            exit(EXIT_FAILURE);
                        }
        case BLACK_i:   switch (piece->kind)
                        {
                            case PAWN:    return BLACK_PAWN;
                            case KNIGHT:  return BLACK_KNIGHT;
                            case BISHOP:  return BLACK_BISHOP;
                            case ROOK:    return BLACK_ROOK;
                            case QUEEN:   return BLACK_QUEEN;
                            case KING:    return BLACK_KING;
                            default:        printf("error: %s: unkown piece; terminating\n", __func__);
                                            exit(EXIT_FAILURE);
                        }
        default:        printf("error: %s: unkown piece; terminating\n", __func__);
                        exit(EXIT_FAILURE);
    }
}

/********************************************************************
 * letter_to_piece: Converts an int (Letter_piece) representation   *
 *                  of a piece which is used for writing            *
 *                  chess-debugging-scripts into the internally     *
 *                  used struct (Piece_i) representation of a piece *
 *                  type.                                           *
 ********************************************************************/
PRIVATE Piece_i letter_to_piece(const Letter_piece letter)
{
    switch (letter)
    {
        case NONE_EMPTY:    return (Piece_i) {NONE_i, EMPTY};
        case WHITE_PAWN:    return (Piece_i) {WHITE_i, PAWN};
        case WHITE_KNIGHT:  return (Piece_i) {WHITE_i, KNIGHT};
        case WHITE_BISHOP:  return (Piece_i) {WHITE_i, BISHOP};
        case WHITE_ROOK:    return (Piece_i) {WHITE_i, ROOK};
        case WHITE_QUEEN:   return (Piece_i) {WHITE_i, QUEEN};
        case WHITE_KING:    return (Piece_i) {WHITE_i, KING};
        case BLACK_PAWN:    return (Piece_i) {BLACK_i, PAWN};
        case BLACK_KNIGHT:  return (Piece_i) {BLACK_i, KNIGHT};
        case BLACK_BISHOP:  return (Piece_i) {BLACK_i, BISHOP};
        case BLACK_ROOK:    return (Piece_i) {BLACK_i, ROOK};
        case BLACK_QUEEN:   return (Piece_i) {BLACK_i, QUEEN};
        case BLACK_KING:    return (Piece_i) {BLACK_i, KING};
        default:                printf("error: %s: unknown letter representation of piece; terminating\n", __func__);
                                exit(EXIT_FAILURE);
    }
}

/********************************************************************
 * set_game_state: Takes a Game_state and sets it's values to       *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_state.                            *
 ********************************************************************/
PRIVATE void set_game_state(Game_state *state, const Letter_piece *board)
{
    state->move_number = 1;
    state->uneventful_moves = 0;
    state->board_occurences = 1;

    state->castle_kngsde_legal_white = true;
    state->castle_qensde_legal_white = true;
    state->castle_kngsde_legal_black = true;
    state->castle_qensde_legal_black = true;

    //state->last_move = (Move_i) { (Square_i) {0,0}, (Square_i) {0,0} };

    board_from_string(state, board);

    // set possible_moves for active player
    state->possible_moves_number = 0;
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
    update_possible_moves_game(state);

    //check if passive player can upgrade pawn
    Color_i passive_player = player_passive(state);
    int pawn_upgrade_row = (WHITE_i == passive_player) ? BOARD_ROWS : 0;
    state->pawn_upgradable = false;
    for (int i = 0; i < BOARD_COLUMNS; i++)
    {
        if ((PAWN == state->board[pawn_upgrade_row][i].kind)
         && (passive_player == state->board[pawn_upgrade_row][i].color))
        {
            state->pawn_upgradable = true;
        }
    }

    state->previous_state = NULL;
}
