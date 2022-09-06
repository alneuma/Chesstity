#ifndef CHESS_BASIC_FUNCTIONS_H
#define CHESS_BASIC_FUNCTIONS_H

struct game_state
{
    int move_number;
    enum {NONE, WHITE, BLACK, BOTH,} rochade_possible;
    int rule_50_moves;
}

#endif
