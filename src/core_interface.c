#include "core_functions.h"
#include "core_interface.h"
#include <stdlib.h>
#include <stdio.h>

struct game {
    Game_state *current_state;
};

Game create_game(void)
{
    Game new_game = malloc(sizeof(*new_game));
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

    set_game_state(beg_state, "rbnqknbr"
                              "pppppppp"
                              "........"
                              "........"
                              "........"
                              "........"
                              "PPPPPPPP"
                              "RBNQKNBR");

    new_game->current_state = beg_state;

    return new_game;
}

void destroy_game(Game game)
{
    Game_state *temp;

    while (NULL != game->current_state->previous_game_state)
    {
        temp = game->current_state;
        game->current_state = game->current_state->previous_game_state;
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
void board_from_string(Game_state *state, Letter_piece *board_string)
{
    char *p = board_string;
    int i = BOARD_ROWS;
    int j;
    while (*p)
    {
        if ((j = (p - board_string) % BOARD_COLUMNS) == 0)
            i--;

        if (LET_WHITE_KING == *p)
            state->king_white = (Square) {i,j};
        else if (LET_BLACK_KING == *p)
            state->king_black = (Square) {i,j};
        
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
 * set_game_state: Takes a Game_state and sets it's values to       *
 *                 default testing value.                           *
 *                 Ignores last_move, king_white, king_black and    *
 *                 *previous_game_state.                            *
 ********************************************************************/
void set_game_state(Game_state *state, Letter_piece *board)
{
    state->move_number = 1;
    state->uneventful_moves = 0;
    state->board_occurences = 1;

    state->castle_kngsde_legal_white = true;
    state->castle_qensde_legal_white = true;
    state->castle_kngsde_legal_black = true;
    state->castle_qensde_legal_black = true;

    //state->last_move = (Move) { (Square) {0,0}, (Square) {0,0} };

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
    Color passive_player = player_passive(state);
    int pawn_upgrade_row = (WHITE == passive_player) ? BOARD_ROWS : 0;
    state->pawn_upgradable = false;
    for (int i = 0; i < BOARD_COLUMNS; i++)
    {
        if ((PAWN == state->board[pawn_upgrade_row][i].kind)
         && (passive_player == state->board[pawn_upgrade_row][i].color))
        {
            state->pawn_upgradable = true;
        }
    }

    state->previous_game_state = NULL;
}
