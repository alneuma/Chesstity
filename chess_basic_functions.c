/* TODO:
 * is_check
 * en passant
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define PUBLIC
#define PRIVATE static
#define BOARD_ROWS 8
#define BOARD_COLUMNS 8
#define WHITE_PAWN_STARTING_ROW 1
#define BLACK_PAWN_STARTING_ROW 6
#define KINGS_STARTING_ROW 4

typedef enum color
{
    NONE,
    BLACK,
    WHITE,
} Color;

/*
 * The first five bits give the kind of piece.
 * The sixth bit gives the color 0 = black, 1 = white.
 * Except for the bishop a figures number is it's value.
 */
typedef enum piece
{
    EMPTY = '*',
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
} Piece;

typedef struct square
{
    int row;
    int column;
} Square;

typedef struct move
{
    Square from;
    Square to;
} Move;

typedef struct board_state
{
    Piece board[BOARD_ROWS][BOARD_COLUMNS];
    Square white_king_position;
    Square black_king_position;
    struct board_state *previous;
} Board_state,

int move_number = 1;
Color rochade_possible = BOTH;
int rule_50_moves = 0;
Move *last_move = NULL;

Board_state current_state =
    { .board[BOARD_ROWS][BOARD_COLUMNS] =
        { {WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK},
          {WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN},
          {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, 
          {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, 
          {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, 
          {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}, 
          {BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN},
          {BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK} };
      .white_king_position = { .row = 0, .column = KINGS_STARTING_ROW };
      .black_king_position = { .row = BOARD_ROWS-1, .column = KINGS_STARTING_ROW };
      .previous = NULL;
    };

/************************************************************
 * reset_game: Resets the game.                             *
 ************************************************************/
void reset_game(void)
{
    Board_state *temp;
    while (NULL != current_state->previous)
    {
        temp = current_state;
        current_state = current_state->previous;
        free(temp);
    }
}

/************************************************************
 * what_color: Returns the color of the piece at a given    *
 *             square of a given board state                *
 ************************************************************/
Color check_color(Square *square, Board_state *state)
{
    switch (state->board[square->row][square->column])
    {
        case EMPTY:         return NONE;
        case WHITE_PAWN:
        case WHITE_KNIGHT:
        case WHITE_BISHOP:
        case WHITE_ROOK:
        case WHITE_QUEEN:
        case WHITE_KING:    return WHITE;
        case BLACK_PAWN:
        case BLACK_KNIGHT:
        case BLACK_BISHOP:
        case BLACK_ROOK:
        case BLACK_QUEEN:
        case BLACK_KING:    return BLACK;
        default:            printf("error: %s: invalid color; terminating\n", __func__);
                            exit(EXIT_SUCCESS);
    }
}

/************************************************************
 * move_legal: Checks if a move on a boardstate is legal.   *
 *             Does not consider if it leads to check for   *
 *             the moving player. This is because to check  *
 *             for check the post-move boardstate needs to  *
 *             be analysed. move_legal() should be applied, *
 *             before a post-move boardstate is created.    *
 ************************************************************/
bool move_legal(Move *move, Board_state *state)
{
    /* checks if starting square is occupied */
    if (state->board[move->from.row][move->from.column] == EMPTY)
        return false;

    /* checks if pieces at starting square and at destination square are of different color */
    if (what_color(move->from) == what_color(move->to))
        return false;

    /* check if piece is to move at all and if it is to moving to an existing square */
    if (((move->from.row == move->to.row) && (move->from.column == move->to.column))
      || (move->to.row < 0)
      || (move->to.row >= BOARD_ROWS)
      || (move->to.column < 0)
      || (move->to.column < BOARD_COLUMNS))
        return false;

    switch (state->board[move->from.row][move->from.column])
    {
        case WHITE_PAWN:    return move_pawn_legal(move, state);
        case BLACK_PAWN:    return move_pawn_legal(move, state);
        case WHITE_KNIGHT:  return move_knight_legal(move, state);
        case BLACK_KNIGHT:  return move_knight_legal(move, state); case WHITE_BISHOP:  return move_bishop_legal(move, state);
        case BLACK_BISHOP:  return move_bishop_legal(move, state);
        case WHITE_ROOK:    return move_rook_legal(move, state);
        case BLACK_ROOK:    return move_rook_legal(move, state);
        case WHITE_QUEEN:   return (move_bishop_legal(move, state) || move_rook_legal(move, state));
        case BLACK_QUEEN:   return (move_bishop_legal(move, state) || move_rook_legal(move, state));
        case WHITE_KING:    return move_king_legal(move, state);
        case BLACK_KING:    return move_king_legal(move, state);
        default:            printf("error: %s: invalid move or board; terminating\n", __func__);
                            exit(EXIT_SUCCESS);
    }
}

/************************************************************
 * move_pawn_legal: Does not consider if move leads to      *
 *                  check for the moving player.            *
 ************************************************************/
PRIVATE bool move_pawn_legal(Move *move, Board_state *state)
{
    /* determening the pawns starting row and into which direction it may move */
    int move_direction, starting_row; 

    if (what_color(move->from) == WHITE)
    {
        move_direction = 1;
        starting_row = WHITE_PAWN_STARTING_ROW;
    }
    else if (what_color(move->from) == BLACK)
    {
        move_direction = -1;
        starting_Kow = BLACK_PAWN_STARTING_ROW;
    }
    else
    {
        printf("error: %s: invalid piece color; terminating\n", __func__);
        exit(EXIT_SUCCESS);
    }

    if (move->to.column == move->from.column)
    {
        if (move->to.row == move->from.row + move_direction)
            return true;
        else if ((move->from.row == starting_row)
              && (move->to.row == starting_row + (2 * move_direction))
              && (EMPTY == state->board[starting_row + move_direction][move->from.column]))
            return true;
    }
    else if (((move->to.column == move->from.column + 1)
           || (move->to.column == move->from.column - 1))
           &&
              (move->to.row == move->from.row + move_direction))
    {
        if ((what_color(move->to) != EMPTY) && (what_color(move->to) != what_color(move->from)))
            return true;
        /*
        else if (en passant)
            return true;
        */
    }
}

/************************************************************
 * move_knight_legal: Checks as if the board was empty.     *
 ************************************************************/
PRIVATE bool move_knight_legal(Move *move, Board_state *state)
{
    return ((((move->to.column == move->from.column - 1) || (move->to.column == move->from.column + 1)) 
          && ((move->to.row == move->from.row - 2) || (move->to.row == move->from.row + 2)))
          ||
            (((move->to.column == move->from.column - 2) || (move->to.column == move->from.column + 2))
          && ((move->to.row == move->from.row - 1) || (move->to.row == move->from.row + 1))));
}

/************************************************************
 * move_bishop_legal: Checks as if the board was empty.     *
 ************************************************************/
PRIVATE bool move_bishop_legal(Move *move, Board_state *state)
{
    int row_step = (move->from.row < move->to.row) ? 1 : -1;
    int column_step = (move->from.column < move->to.column) ? 1 : -1;

    int row = move->from.row + row_step;
    int column = move->from.column + column_step;

    while ((row != move->to.row) && (column != move->to.column))
    {
        if (EMPTY != state->board[row][column])
            return false;

        row += row_step;
        column += column_step;
    }

    return (row == move->to.row) && (column == move->to.column);
}

/************************************************************
 * move_rook_legal: Checks as if the board was empty.       *
 ************************************************************/
PRIVATE bool move_rook_legal(Move *move, Board_state *state)
{
    int row_step = (move->from.row < move->to.row) ? 1 : -1;
    int column_step = (move->from.column < move->to.column) ? 1 : -1;

    if (((row_step + column_step) != 1) && ((row_step + column_step) != -1))
        return false;

    int row = move->from.row + row_step;
    int column = move->from.column + column_step;
    
    while ((row != move->to.row) || (column != move->to.column))
    {
        if (EMPTY != state->board[row][column])
            return false;

        row += row_step;
        column += column_step;
    }

    return true;
}
    
/************************************************************
 * move_king_legal: Checks as if the board was empty.       *
 ************************************************************/
PRIVATE bool move_king_legal(Move *move, Board_state *state)
{
    return ((((move->from.column - move->to.column) == 1)
          || ((move->from.column - move->to.column) == -1))
          &&
            (((move->from.row - move->to.row) == 1)
          || ((move->from.row - move->to.row) == -1)));
}

/************************************************************
 * is_check: Checks if the king of a given color is in      *
 *           check.                                         *
 ************************************************************/
bool is_check(Color color, Board_state *state)
{
    Square king;
    Color hostile_color;
    if (WHITE == color)
    {
        Square king = state->white_king_position;
        hostile_color = BLACK;
    }
    else if (BLACK == color)
    {
        Square king = state->black_king_position;
        hostile_color = WHITE;
    }
    else
    {
        printf("error: %s: color requested; terminating\n", __func__);
        exit(EXIT_SUCCESS);
    }

    int row;
    int column = king->column;

    /* check above */
    for (row = king->row + 1; row < BOARD_ROWS; ++row)
    {
        if (EMPTY != what_color(state->board[row][column]));
        {
            if ((((WHITE = hostile_color) ? WHITE_ROOK : BLACK_ROOK) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_QUEEN : BLACK_QUEEN) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_KING : BLACK_KING) == state->board[row][column]) && (row == king.row + 1))
                return true;
            else 
                break;
        }
    }
                
    /* check below */
    for (row = king->row - 1; row >= 0; --row)
    {
        if (EMPTY != what_color(state->board[row][column]));
        {
            if ((((WHITE = hostile_color) ? WHITE_ROOK : BLACK_ROOK) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_QUEEN : BLACK_QUEEN) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_KING : BLACK_KING) == state->board[row][column]) && (row == king.row - 1))
                return true;
            else 
                break;
        }
    }

    /* check left */
    row = king->row;
    for (column = king->column - 1; column >= 0; --column)
    {
        if (EMPTY != what_color(state->board[row][column]));
        {
            if ((((WHITE = hostile_color) ? WHITE_ROOK : BLACK_ROOK) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_QUEEN : BLACK_QUEEN) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_KING : BLACK_KING) == state->board[row][column]) && (column == king.column - 1))
                return true;
            else
                break;
        }
    }

    /* check right */
    for (column = king->column + 1; column < BOARD_COLUMNS; ++column)
    {
        if (EMPTY != what_color(state->board[row][column]));
        {
            if ((((WHITE = hostile_color) ? WHITE_ROOK : BLACK_ROOK) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_QUEEN : BLACK_QUEEN) == state->board[row][column])
             || (((WHITE = hostile_color) ? WHITE_KING : BLACK_KING) == state->board[row][column]) && (column == king.column + 1))
                return true;
            else
                break;
        }
    }
                

        






/************************************************************
 * apply_move: Returns a the board state after a move is    *
 *             applied. Calls error_out_of_memory() if      *
 *             memory allocation fails.                     *
 *             Returns NULL if move is not legal at current *
 *             board state.                                 *
 ************************************************************/
Board_state *apply_move(Move *move, Board_state *current_state)
{
    if (!move_legal(move, current_state))
        return NULL;

    Board_state *new_state = malloc(sizeof(*new_state));
    if (NULL == new_state)
        error_out_of_memory();

    *new_state = *current_state;
    new_state->board[move->to.row][move->to.column] = current_state->board[move->from.row][move->from.column];
    new_state->board[move->from.row][move->from.column] = EMPTY;

    if (is_check(to_move(move_number), new_state))
    {
        free(new_state);
        return NULL;
    }

    return new_state;
}

/************************************************************
 * error_out_of_memory: 
 ************************************************************/
void error_out_of_memory(void)
{
    printf("error: out of memory; terminating\n");
    exit(EXIT_SUCCESS);
}

/************************************************************
 * to_move: Returns the color of the player to move.        *
 ************************************************************/
Color to_move(int move_number)
{
    return (move_number % 2) + 1;
}
