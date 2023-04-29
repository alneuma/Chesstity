// Copyright: (c) 2023, Alrik Neumann
// GNU General Public License v3.0+ (see LICENSE.txt or https://www.gnu.org/licenses/gpl-3.0.txt)

// settings to enable debugging
#define DEBUG
#ifndef DEBUG
#define PRIVATE static
#else
#define PRIVATE
#endif

#include "core_functions.h"
#include "core_interface.h"
#include "graphic_output.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

PRIVATE char intern_to_output_symbol(const Letter_piece intern);

/********************************************************************
 * intern_to_output_symbol: Uses one the symbols internally
 *                          representing chess pieces and translates
 *                          it into the symbol wich should be
 *                          displayed on the interface.
 *                          See core_interface.h for a list of the
 *                          symbols used internally.
 ********************************************************************/
PRIVATE char intern_to_output_symbol(const Letter_piece intern)
{
    switch (intern)
    {
        case '.':   return ' ';
        case 'P':   return 'P';
        case 'N':   return 'N';
        case 'B':   return 'B';
        case 'R':   return 'R';
        case 'Q':   return 'Q';
        case 'K':   return 'K';
        case 'p':   return 'p';
        case 'n':   return 'n';
        case 'b':   return 'b';
        case 'r':   return 'r';
        case 'q':   return 'q';
        case 'k':   return 'k';
        default:    printf("error: %s: no rule to translate internal symbol into output character; terminating\n", __func__);
                    exit(EXIT_SUCCESS);
    }
}

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

/********************************************************************
 * write_current_board: Writes the board of game into dest in the
 *                      way it should be displayed on the screen.
 *                      orientation, as the name suggests, gives the
 *                      orientation of the displayed board, which
 *                      either should be WHITE or BLACK.
 *                      If valid orientation is provided, it will
 *                      default to WHITE.
 ********************************************************************/
void write_current_board(char *dest, Game game, Color orientation)
{
    const Letter_piece *board = current_board(game);
    *dest = '\0';
    char buffer[1000];

    strncat(dest, " ", 1000);
    int i, j;
    for (i = 0; i <= 7; i++)
    {
        sprintf(buffer, "    %c ", (BLACK == orientation) ? ('h' - i) : ('a' + i));
        strncat(dest, buffer, 1000);
    }

    sprintf(buffer, " \n  _________________________________________________ \n");
    strncat(dest,buffer, 1000);
    for (i = 0; i <= 7; i++)
    {
        sprintf(buffer, "  |     |     |     |     |     |     |     |     | \n%d |",
                (BLACK == orientation) ? (i + 1) : (8 - i));
        strncat(dest, buffer, 1000);

        for (j = 0; j <= 7; j++)
        {
            sprintf(buffer, "  %c  |",
                    intern_to_output_symbol(*(board + ((BLACK == orientation) ? ((7-i)*8+j) : (8*i+j)))));
            strncat(dest, buffer, 1000);
        }

        sprintf(buffer, " %d\n  |_____|_____|_____|_____|_____|_____|_____|_____| \n",
                (BLACK == orientation) ? (i + 1) : (8 - i));
        strncat(dest, buffer, 1000);
    }

    strncat(dest, "\n", 1000);
    for (i = 0; i <= 7; i++)
    {
        sprintf(buffer, "     %c", (BLACK == orientation) ? ('h' - i) : ('a' + i));
        strncat(dest, buffer, 1000);
    }
    strncat(dest, "\n", 1000);
}
