#include "core_functions.h"
#include "graphic_output.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/********************************************************************
 * piece_to_letter: Returns the letter associated with a piece.     *
 ********************************************************************/
char piece_to_letter(Piece_i *piece)
{
    char letter;
    switch (piece->kind)
    {
        case EMPTY:     return ' ';
        case PAWN:      letter = 'P';
                        break;
        case KNIGHT:    letter = 'N';
                        break;
        case BISHOP:    letter = 'B';
                        break;
        case ROOK:      letter = 'R';
                        break;
        case QUEEN:     letter = 'Q';
                        break;
        case KING:      letter = 'K';
                        break;
        default:        printf("error: %s: unknown kind of piece; terminating\n", __func__);
                        exit(EXIT_SUCCESS);
    }

    return (BLACK_i == piece->color) ? tolower(letter) : letter;
}

/********************************************************************
 * print_board: Prints game_state->board as an ascii chess-board.   *
 ********************************************************************/
void print_board(Game_state *game_state)
{
    int i, j;
    printf(" _________________________________________________\n");
    for (i = 7; i >= 0; i--)
    {
        printf(" |     |     |     |     |     |     |     |     |\n%d|", i + 1);

        for (j = 0; j<= 7; j++)
            printf("  %c  |", piece_to_letter(&game_state->board[i][j]));

        printf("\n |_____|_____|_____|_____|_____|_____|_____|_____|\n");
    }

    printf("    a     b     c     d     e     f     g     h\n");
}
