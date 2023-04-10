#include "san_parsing.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PRIVATE static

PRIVATE san_type san_type_pawn(const char *input);
PRIVATE san_type san_type_piece(const char *input);
PRIVATE san_type san_type_castle(const char *input);

/////////////////////////////////////////////////////////////////////
// trim_whitespace(): 
/////////////////////////////////////////////////////////////////////
void trim_whitespace(char *string)
{
    // skip initial whitespace
    char *p = string;
    while (isspace(*p))
        p++;
    if ('\0' == *p)
    {
        *string = '\0';
        return;
    }
    
    // walk through the rest until '\0' reached
    // mark first non-space and first trailing space
    char *first_non_space = p;
    char *first_trailing_space;
    for (;;)
    {
        // walk through sequence of non-spaces
        while (('\0' != *p) && !isspace(*p))
            p++;
        first_trailing_space = p;
        if ('\0' == *p)
            break;

        // walk through sequence of spaces
        while (('\0' != *p) && isspace(*p))
            p++;
        if ('\0' == *p)
        {
            if (!isspace(*(p-1)))
                first_trailing_space = p;
            break;
        }
    }

    if (string == first_non_space)
    {
        *first_trailing_space = '\0';   
    }
    else
    {
        while (first_non_space < first_trailing_space)
        {
            *string++ = *first_non_space++;
        }
        *string = '\0';
    }
}

/////////////////////////////////////////////////////////////////////
// get_san_type(): 
/////////////////////////////////////////////////////////////////////
san_type get_san_type(const char *input)
{
    san_type san;
    if ((san = san_type_pawn(input)))
        return san;
    else if ((san = san_type_piece(input)))
        return san;
    else
        return san_type_castle(input);
}

PRIVATE san_type san_type_pawn(const char *input)
{
    switch (*input)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
            switch (*(input+1))
            {
                case '1':
                case '8':
                    // if pawn reaches first or last row, check if the trade-in-piece is specified
                    if ('=' == *(input+2))
                        switch (*(input+3))
                        {
                            case 'N':
                            case 'B':
                            case 'R':
                            case 'Q':
                                if ('\0' == *(input+4))
                                    return PAWN_MOVE_PROMOTE;
                            default:
                                return INVALID_SAN;
                        }
                    else
                        return INVALID_SAN;
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    if ('\0' == *(input+2))
                        return PAWN_MOVE;
                    else if ('x' != *(input+1))
                        return INVALID_SAN;
                case 'x':
                    // if pawn captures
                    // first make sure that columns are neighbours
                    if (1 != abs(*input - *(input+2)))
                        return INVALID_SAN;
                    switch (*(input+2))
                    {
                        case 'a':
                        case 'b':
                        case 'c':
                        case 'd':
                        case 'e':
                        case 'f':
                        case 'g':
                        case 'h':
                            switch (*(input+3))
                            {
                                // if reaches first or last column, check if trade-in-piece is specified
                                case '1':
                                case '8':
                                    if ('=' == *(input+4))
                                        switch (*(input+5))
                                        {
                                            case 'N':
                                            case 'B':
                                            case 'R':
                                            case 'Q':
                                                if ('\0' == *(input+6))
                                                    return PAWN_CAPTURE_PROMOTE;
                                            default:
                                                return INVALID_SAN;
                                        }
                                    else
                                        return INVALID_SAN;
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                    if ('\0' == *(input+4))
                                        return PAWN_CAPTURE;
                                default:
                                    return INVALID_SAN;
                            }
                        default:
                            return INVALID_SAN;
                    }
                default:
                    return INVALID_SAN;
            }
        default:
            return INVALID_SAN;
    }
}

PRIVATE san_type san_type_piece(const char *input)
{
    switch (*input)
    {
        case 'N':
        case 'B':
        case 'R':
        case 'Q':
        case 'K':
            if ((*(input + 1) >= 'a') &&
                (*(input + 1) <= 'h'))
            {
                if ((*(input + 2) >= '1') &&
                    (*(input + 2) <= '8'))
                {

                    if ('\0' == *(input + 3))
                        return PIECE_MOVE;
                    else if ((*(input + 3) >= 'a') &&
                             (*(input + 3) <= 'h') &&
                             (*(input + 4) >= '1') &&
                             (*(input + 4) <= '8') &&
                             ('\0' == *(input + 5)))
                        return PIECE_MOVE_BOTH;
                    else if (('x' == *(input + 3)) &&
                             (*(input + 4) >= 'a') &&
                             (*(input + 4) <= 'h') &&
                             (*(input + 5) >= '1') &&
                             (*(input + 5) <= '8') &&
                             ('\0' == *(input + 6)))
                        return PIECE_CAPTURE_BOTH;
                    else
                        return INVALID_SAN;
                }
                else if ((*(input + 2) >= 'a') &&
                         (*(input + 2) <= 'h'))
                {
                    if ((*(input + 3) >= '1') &&
                        (*(input + 3) <= '8') &&
                        ('\0' == *(input + 4)))
                        return PIECE_MOVE_FILE;
                    else
                        return INVALID_SAN;
                }
                else if (('x' == *(input + 2)) &&
                         (*(input + 3) >= 'a') &&
                         (*(input + 3) <= 'h') &&
                         (*(input + 4) >= '1') &&
                         (*(input + 4) <= '8') &&
                         ('\0' == *(input + 5)))
                    return PIECE_CAPTURE_FILE;
            }
            else if ((*(input + 1) >= '1') &&
                     (*(input + 1) <= '8'))
            {
                if ((*(input + 2) >= 'a') &&
                    (*(input + 2) <= 'h') &&
                    (*(input + 3) >= '1') &&
                    (*(input + 3) <= '8') &&
                    ('\0' == *(input + 4)))
                    return PIECE_MOVE_RANK;
                else if (('x' == *(input + 2)) &&
                         (*(input + 3) >= 'a') &&
                         (*(input + 3) <= 'h') &&
                         (*(input + 4) >= '1') &&
                         (*(input + 4) <= '8') &&
                         ('\0' == *(input + 5)))
                         return PIECE_CAPTURE_RANK;
            }
            else if (('x' == *(input + 1)) &&
                     (*(input + 2) >= 'a') &&
                     (*(input + 2) <= 'h') &&
                     (*(input + 3) >= '1') &&
                     (*(input + 3) <= '8') &&
                     ('\0' == *(input + 4)))
                     return PIECE_CAPTURE;
        default:
            return INVALID_SAN;
    }
}

PRIVATE san_type san_type_castle(const char *input)
{
    if (('O' == *input) &&
        ('-' == *(input + 1)) &&
        ('O' == *(input + 2)))
    {
        if ('\0' == *(input + 3))
            return CASTLE_RIGHT;
        else if (('-' == *(input + 3)) &&
                 ('O' == *(input + 4)) &&
                 ('\0' == *(input + 5)))
            return CASTLE_LEFT;
    }
    return INVALID_SAN;
}
