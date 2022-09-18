#include "core_functions.h"
#include "graphic_output.h"
#include "chess_test_creator.h"

int main(void)
{
    Game_state state;

    Letter_piece board_string[] = "........"
                                  "....r..."
                                  "........"
                                  "........"
                                  "..P.R..."
                                  "........"
                                  "........"
                                  ".K......";

    board_from_string(&state, board_string);

    print_board(&state);

    return 0;
}
