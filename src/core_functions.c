#include "core_functions.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/********************************************************************
 * board_repeated: Counts the number of times the actual board-     *
 *                 state occured before in the game.                *
 *                 Is necessary to determine if draw can be         *
 *                 claimed.                                         *
 ********************************************************************/
bool compare_boards(Piece board_1[BOARD_ROWS][BOARD_COLUMNS], Piece board_2[BOARD_ROWS][BOARD_COLUMNS])
{
    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; j < BOARD_COLUMNS; ++j)
        {
            if ((board_1[i][j].color != board_2[i][j].color)
             || (board_1[i][j].kind != board_2[i][j].kind))
                return false;
        }
    }
    return true;
}

/********************************************************************
 * board_repeated: Counts the number of times the actual board-     *
 *                 state occured before in the game.                *
 *                 Is necessary to determine if draw can be         *
 *                 claimed.                                         *
 ********************************************************************/
int board_repeated(Game_state *state)
{
    Game_state *ptr = state;

    while (ptr->move_number > 2)
    {
        // It is enough to examine every second boards-state because two of them are not the same, when different players are to move.
        ptr = ptr->previous_game_state->previous_game_state;

        // Here more equalities are checked than would be strictly necessary.
        // I do this to provoce operator short-circuting (and thus hopefully check less equalities).
        if ((ptr->possible_moves_number == state->possible_moves_number)
         && (ptr->castle_kngsde_legal_white == state->castle_kngsde_legal_white)
         && (ptr->castle_qensde_legal_white == state->castle_qensde_legal_white)
         && (ptr->castle_kngsde_legal_black == state->castle_kngsde_legal_black)
         && (ptr->castle_qensde_legal_black == state->castle_qensde_legal_black)
         && (compare_boards(ptr->board, state->board)))
            return 1 + ptr->board_occurences;
    }

    return 1;
}

/********************************************************************
 * apply_move: Returns a pointer to a dynamically allocated         *
 *             Game_state which represents the state of the game    *
 *             after move was performed in state.                   *
 *             Returns NULL if memoryallocation fails.              *
 *             Assumes that move is legal.                          *
 ********************************************************************/
Game_state *apply_move(Game_state *state, Move move)
{
    Game_state *new_state = malloc(sizeof(*new_state));
    if (NULL == new_state)
        return NULL;

    *new_state = *state;
    new_state->previous_game_state = state;

    Color moving_player = player_active(state);

    // processing pawn-move-effects
    // and updating new_state->uneventful_moves
    new_state->pawn_upgradable = false;
    if (PAWN == state->board[move.from.row][move.from.column].kind)
    {
        // updating uneventful_moves through pawn-move
        new_state->uneventful_moves = 0;

        // checking if move is en passant capturing and removing captured pawn if it is
        if ((move.from.column != move.to.column)
         && (EMPTY == state->board[move.to.row][move.to.column].kind))
        {
            new_state->board[move.from.row][move.to.column] = (Piece) {NONE, EMPTY};
        }
        // checking if pawn can be upgraded (can't happen in the same turn as en passant capturing)
        else if ((0 == move.to.row) || (BOARD_ROWS - 1 == move.to.row))
        {
            new_state->pawn_upgradable = true;
        }
    }
    // updating uneventful_moves through capturing
    else if (EMPTY != state->board[move.to.row][move.to.column].kind)
    {
        new_state->uneventful_moves = 0;
    }
    // updating uneventful_moves if nothing spectacular happened
    else
    {
        ++new_state->uneventful_moves;
    }

    // processing king move effects
    if (KING == state->board[move.from.row][move.from.column].kind)
    {
        // moving rook in case of kingside castling
        if (move.from.column + 2 == move.to.column)
        {
            new_state->board[move.from.row][move.from.column + 1] = (Piece) {moving_player, ROOK};
            new_state->board[move.from.row][BOARD_COLUMNS - 1] = (Piece) {NONE, EMPTY};
        }
        // moving rook in case of queenside castling
        else if (move.from.column - 2 == move.to.column)
        {
            new_state->board[move.from.row][move.from.column - 1] = (Piece) {moving_player, ROOK};
            new_state->board[move.from.row][0] = (Piece) {NONE, EMPTY};
        }

        // updating king squares and future castling legality
        if (WHITE == moving_player)
        {
            new_state->king_white = (Square) {move.to.row, move.to.column};
            new_state->castle_kngsde_legal_white = false;
            new_state->castle_qensde_legal_white = false;
        }
        else if (BLACK == moving_player)
        {
            new_state->king_black = (Square) {move.to.row, move.to.column};
            new_state->castle_kngsde_legal_black = false;
            new_state->castle_qensde_legal_black = false;
        }
    }
    // updating future castling legality in case of rook-move
    else if ((ROOK == state->board[move.from.row][move.from.column].kind)
          && (move.from.row == ((WHITE == moving_player) ? 0 : BOARD_ROWS - 1)))
    {
        if (WHITE == moving_player)
        {
            if (move.from.column == BOARD_COLUMNS - 1)
                new_state->castle_kngsde_legal_white = false;
            else if (move.from.column == 0)
                new_state->castle_qensde_legal_white = false;
        }
        else if (BLACK == moving_player)
        {
            if (move.from.column == BOARD_COLUMNS - 1)
                new_state->castle_kngsde_legal_black = false;
            else if (move.from.column == 0)
                new_state->castle_qensde_legal_black = false;
        }
    }

    // move the moving piece
    new_state->board[move.to.row][move.to.column] = new_state->board[move.from.row][move.from.column];
    new_state->board[move.from.row][move.from.column] = (Piece) {NONE, EMPTY};

    // update remaining variablies in new_state
    ++new_state->move_number;
    new_state->possible_moves_number = 0;
    update_possible_moves_game(new_state);
    new_state->last_move = (Move) {(Square) {move.from.row, move.from.column}, (Square) {move.to.row, move.to.column}};

    new_state->board_occurences = board_repeated(new_state);

    return new_state;
}

/********************************************************************
 * player_active: Returns the color of the active player.           *
 ********************************************************************/
Color player_active(Game_state *state)
{
    return (state->move_number - 1) % 2 + 1;
}

/********************************************************************
 * player_passive: Returns the color of the active player.          *
 ********************************************************************/
Color player_passive(Game_state *state)
{
    return state->move_number % 2 + 1;
}

/********************************************************************
 * king_square: Returns a pointer to the square of players's king   *
 *              at a given Game_state.                              *
 *              This function exists to have an easy way to access  *
 *              the position of a players king without the need of  *
 *              conditional statements in the code.                 *
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
 * update_possible_moves_game: Looks at certain parameters of a     *
 *                             Game_state structure and writes it's *
 *                             possible moves.                      *
 ********************************************************************/
void update_possible_moves_game(Game_state *state)
{
    Color active_player = player_active(state);
    for (int i = 0; i < BOARD_ROWS; ++i)
    {
        for (int j = 0; j < BOARD_COLUMNS; ++j)
        {
            if (state->board[i][j].color == active_player)
            {
                write_possible_moves_square(state, (Square) {i,j});
            }
        }
    }
}

/********************************************************************
 * write_possible_moves_square: writes state->possible_moves        *
 *                              and adds to                         *
 *                              state->possible_moves_number        *
 *                              for the piece on one square.        *
 *                                                                  *
 *                              IMPORTANT:                          *
 *                              Assumes that all entries of         *
 *                              state->possible moves are set       *
 *                              to false.                           *
 ********************************************************************/
void write_possible_moves_square(Game_state *state, Square square)
{
    Color active_player = player_active(state);
    if (state->board[square.row][square.column].color != active_player)
        return;
    switch (state->board[square.row][square.column].kind)
    {
        case PAWN:      write_pawn_possible_moves(state, &square);
                        break;
        case KNIGHT:    write_knight_possible_moves(state, &square);
                        break;
        case BISHOP:    write_bishop_possible_moves(state, &square);
                        break;
        case ROOK:      write_rook_possible_moves(state, &square);
                        break;
        case QUEEN:     write_bishop_possible_moves(state, &square);
                        write_rook_possible_moves(state, &square);
                        break;
        case KING:      write_king_possible_moves(state, &square);
                        break;
        default:        printf("error: %s: invalid kind of piece\n", __func__);
                        exit(EXIT_SUCCESS);
    }
}

/********************************************************************
 * write_pawn_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumption.            *
 ********************************************************************/
void write_pawn_possible_moves(Game_state *state, Square *square)
{
    Color active_player = player_active(state);
    Color passive_player = (WHITE == active_player) ? BLACK : WHITE;
    int move_direction = (WHITE == active_player) ? 1 : -1;

    // declaring row and column of target square
    int target_row = square->row + move_direction;
    int target_column = square->column;

    if ((target_row >= 0) && (target_row < BOARD_ROWS))
    {

        // moving one square
        if ((EMPTY == state->board[target_row][target_column].kind)
         && (!in_check_after_move(state, (Move) {*square, (Square) {target_row, target_column}})))
        {
            state->possible_moves[square->row][square->column][target_row][target_column] = true;
            ++state->possible_moves_number;
        }

        // moving two squares
        target_row = square->row + (move_direction * 2);
        if ((((WHITE == active_player) ? 1 : 6) == square->row)    // 1 : 6 == starting rows of white : black pawns
           && (target_row >= 0)
           && (target_row < BOARD_ROWS)
           && (EMPTY == state->board[target_row][target_column].kind)
           && (!in_check_after_move(state, (Move) {*square, (Square) {target_row, target_column}})))
        {
            state->possible_moves[square->row][square->column][target_row][target_column] = true;
            ++state->possible_moves_number;
        }

        // capturing left
        target_row = square->row + move_direction;
        target_column = square->column - 1;
        if ((target_column >= 0)
            &&
              ((passive_player == state->board[target_row][target_column].color)  // regular capturing
            ||
              ((NONE == state->board[target_row][target_column].color)            // en passant capturing
            && (passive_player == state->board[square->row][target_column].color)
            && (PAWN == state->board[square->row][target_column].kind)
            && (state->last_move.to.row == square->row)
            && (state->last_move.to.column == target_column)
            && (state->last_move.from.row == ((WHITE == passive_player) ? 1 : 6))))
            &&
               (!in_check_after_move(state, (Move) {*square, (Square) {target_row, target_column}})))
        {
            state->possible_moves[square->row][square->column][target_row][target_column] = true;
            ++state->possible_moves_number;
        }

        // capturing right
        target_column = square->column + 1;
        if ((square->column + 1 < BOARD_ROWS)
            &&
              ((passive_player == state->board[target_row][target_column].color)  // regular capturing
            ||
              ((NONE == state->board[target_row][target_column].color)            // en passant capturing
            && (passive_player == state->board[square->row][target_column].color)
            && (PAWN == state->board[square->row][target_column].kind)
            && (state->last_move.to.row == square->row)
            && (state->last_move.to.column == target_column)
            && (state->last_move.from.row == ((WHITE == passive_player) ? 1 : 6))))
            &&
               (!in_check_after_move(state, (Move) {*square, (Square) {target_row, target_column}})))
        {
            state->possible_moves[square->row][square->column][target_row][target_column] = true;
            ++state->possible_moves_number;
        }
    }
}

/********************************************************************
 * write_knight_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
void write_knight_possible_moves(Game_state *state, Square *square)
{
    Color active_player = player_active(state);

    // The row- and column-modifier arrays give the positions of candidate squares relative to the starting square.
    // When both arrays are subscripted with the same index and the values are added to the respective values of the starting square,
    // it results a candidate square.
    int row_modifier[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int column_modifier[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
    int row, column;
    for (int i = 0; i < 8; ++i) // 8 == length of modifier arrays
    {
        row = square->row + row_modifier[i];
        column = square->column + column_modifier[i]; 

        if ((row >= 0)
         && (row < BOARD_ROWS)
         && (column >= 0)
         && (column < BOARD_COLUMNS)
         && (active_player != state->board[row][column].color)
         && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
}

/********************************************************************
 * write_bishop_possible_moves: Only gets called by                 *
 *                              write_possible_moves_square().      *
 *                              Makes the same assumptions.         *
 ********************************************************************/
void write_bishop_possible_moves(Game_state *state, Square *square)
{
    Color active_player = player_active(state);

    // upper-left
    int row;
    int column;
    for (row = square->row + 1, column = square->column - 1;
         row < BOARD_ROWS && column >= 0; ++row, --column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if ((row < BOARD_ROWS)
     && (column >= 0)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // upper-right
    for (row = square->row + 1, column = square->column + 1;
         row < BOARD_ROWS && column < BOARD_COLUMNS; ++row, ++column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if ((row < BOARD_ROWS)
     && (column < BOARD_COLUMNS)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // lower-left
    for (row = square->row - 1, column = square->column - 1;
         row >= 0 && column >= 0; --row, --column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if ((row >= 0)
     && (column >= 0)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }

    // lower-right
    for (row = square->row - 1, column = square->column + 1;
         row >= 0 && column < BOARD_COLUMNS; --row, ++column)
    {
        if (EMPTY != state->board[row][column].kind)
            break;

        if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
        {
            state->possible_moves[square->row][square->column][row][column] = true;
            ++state->possible_moves_number;
        }
    }
    if ((row >= 0)
     && (column < BOARD_COLUMNS)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
    {
        state->possible_moves[square->row][square->column][row][column] = true;
        ++state->possible_moves_number;
    }
}

/********************************************************************
 * write_rook_possible_moves: Only gets called by                   *
 *                            write_possible_moves_square().        *
 *                            Makes the same assumptions.           *
 ********************************************************************/
void write_rook_possible_moves(Game_state *state, Square *square)
{
    Color active_player = player_active(state);

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
    if ((column >= 0)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
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
    if ((column < BOARD_COLUMNS)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
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
    if ((row < BOARD_ROWS)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
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
    if ((row >= 0)
     && (active_player != state->board[row][column].color)
     && (!in_check_after_move(state, (Move) {*square, (Square) {row, column}})))
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
void write_king_possible_moves(Game_state *state, Square *square)
{
    Color active_player = player_active(state);

    // standard moves
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
            else if (!in_check_after_move(state, (Move) {*square, (Square) {row, column}}))
            {
                state->possible_moves[square->row][square->column][row][column] = true;
                ++state->possible_moves_number;
            }
        }
    }

    // castling
    if ((WHITE == active_player)
     && (!is_attacked_by(state, (Square) {0,4}, BLACK)))
    {
        // kingside
        if ((true == state->castle_kngsde_legal_white)
         && (ROOK == state->board[0][7].kind)
         && (WHITE == state->board[0][7].color)
         && (EMPTY == state->board[0][5].kind)
         && (EMPTY == state->board[0][6].kind)
         && (!is_attacked_by(state, (Square) {0,6}, BLACK))
         && (!is_attacked_by(state, (Square) {0,5}, BLACK)))
        {
            state->possible_moves[square->row][square->column][0][6] = true;
            ++state->possible_moves_number;
        }

        // queenside
        if ((true == state->castle_qensde_legal_white)
         && (ROOK == state->board[0][0].kind)
         && (WHITE == state->board[0][0].color)
         && (EMPTY == state->board[0][1].kind)
         && (EMPTY == state->board[0][2].kind)
         && (EMPTY == state->board[0][3].kind)
         && (!is_attacked_by(state, (Square) {0,3}, BLACK))
         && (!is_attacked_by(state, (Square) {0,2}, BLACK)))
        {
            state->possible_moves[square->row][square->column][0][2] = true;
            ++state->possible_moves_number;
        }
    }
    else if ((BLACK == active_player)
          && (!is_attacked_by(state, (Square) {7,4}, WHITE)))
    {
        // kingside
        if ((true == state->castle_kngsde_legal_black)
         && (ROOK == state->board[7][7].kind)
         && (BLACK == state->board[7][7].color)
         && (EMPTY == state->board[7][5].kind)
         && (EMPTY == state->board[7][6].kind)
         && (!is_attacked_by(state, (Square) {7,6}, WHITE))
         && (!is_attacked_by(state, (Square) {7,5}, WHITE)))
        {
            state->possible_moves[square->row][square->column][7][6] = true;
            ++state->possible_moves_number;
        }

        // queenside
        if ((true == state->castle_qensde_legal_black)
         && (ROOK == state->board[7][0].kind)
         && (BLACK == state->board[7][0].color)
         && (EMPTY == state->board[7][1].kind)
         && (EMPTY == state->board[7][2].kind)
         && (EMPTY == state->board[7][3].kind)
         && (!is_attacked_by(state, (Square) {7,3}, WHITE))
         && (!is_attacked_by(state, (Square) {7,2}, WHITE)))
        {
            state->possible_moves[square->row][square->column][7][2] = true;
            ++state->possible_moves_number;
        }
    }
}

/********************************************************************
 * in_check_after_move: Checks if the moving player at a given game *
 *                      state would be in check after a potential   *
 *                      move.                                       *
 ********************************************************************/
bool in_check_after_move(Game_state *state, Move move)
{
    Color defending_player = player_active(state);
    Color attacking_player = player_passive(state);

    // create new gamestate to play the move scenario
    Game_state new_state = *state;

    // checking if move is en passant capturing and removing captured pawn if it is
    if ((PAWN == state->board[move.from.row][move.from.column].kind)
     && (move.from.column != move.to.column)
     && (EMPTY == state->board[move.to.row][move.to.column].kind))
    {
        new_state.board[move.from.row][move.to.column] = (Piece) {NONE, EMPTY};
    }

    // performing move (capturing happens through override)
    new_state.board[move.to.row][move.to.column] = new_state.board[move.from.row][move.from.column];
    new_state.board[move.from.row][move.from.column] = (Piece) {NONE, EMPTY};

    // check for check
    if (KING == new_state.board[move.to.row][move.to.column].kind)
    {
        return is_attacked_by(&new_state, move.to, attacking_player);
    }
    return is_attacked_by(&new_state, *king_square(&new_state, defending_player), attacking_player);
}

/********************************************************************
 * is_attacked_by: To see if castling is possible, it is necessary  *
 *                 to check if unoccupied squares are attacked by a *
 *                 specific player. Thats why the color of the      *
 *                 defending player needs to be specified. This     *
 *                 also could prove useful for the AI to plan       *
 *                 moves.                                           *
 ********************************************************************/
bool is_attacked_by(Game_state *game_state, Square square, Color attacking_player)
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
    for (row = square.row - 1; row >= 0; --row)
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
    for (int i = 0; i < 8; ++i) // 8 == length of modifier arrays
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
        (((game_state->last_move.from.row + ((WHITE == attacking_player) ? -2 : 2)) == game_state->last_move.to.row)
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
