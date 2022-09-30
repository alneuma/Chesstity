#include "core_functions.h"
#include "core_interface.h"
#include <stdlib.h>
#include <stdio.h>

// the starting setup of the board
#define STARTING_BOARD "rnbqkbnr"   \
                       "pppppppp"   \
                       "........"   \
                       "........"   \
                       "........"   \
                       "........"   \
                       "PPPPPPPP"   \
                       "RNBQKBNR"

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

struct game
{
    Game_state *current_state;
};

static void set_game_state(Game_state *state, const Letter_piece *board);
static Piece_i letter_to_piece(const Letter_piece letter);

/********************************************************************
 * create_game: Creates a Game object, which is a linked list, with *
 *              variables of type Game_state as nodes.              *
 ********************************************************************/
Game create_game(void)
{
    Game new_game = malloc(sizeof(new_game));
    if (NULL == new_game)
    {
        printf("error: %s: memoryallocation for new game failed", __func__);
        exit(EXIT_FAILURE);
    }

    Game_state *beg_state = malloc(sizeof(*beg_state));
    if (NULL == beg_state)
    {
        printf("error: %s: memoryallocation for new game failed", __func__);
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
    Game_state *temp;

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
static void board_from_string(Game_state *state, const Letter_piece *board_string)
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
 * letter_to_piece: Converts an int (Letter_piece) representation   *
 *                  of a piece which is used for writing            *
 *                  chess-debugging-scripts into the internally     *
 *                  used struct (Piece_i) representation of a piece *
 *                  type.                                           *
 ********************************************************************/
static Piece_i letter_to_piece(const Letter_piece letter)
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
static void set_game_state(Game_state *state, const Letter_piece *board)
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
