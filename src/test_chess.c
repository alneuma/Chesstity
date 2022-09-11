#include "test-framework/unity/unity.h"
#include "test-framework/unity/unity_chess_extension.h"

#include "chess_test_creator.h"
#include <stdbool.h>

/* unit to be tested */
#include "core_functions.h"

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

//////////////////////
// core_functions.h //
//////////////////////
void test_board(void)
{
    Game_state state;
    board_from_string(&state, "********"
                              "********"
                              "********"
                              "********"
                              "****K***"
                              "********"
                              "********"
                              "********");
    TEST_ASSERT_TRUE(WHITE == state.board[3][4].color);
}

void test_is_attacked_rook(void)
{
    Game_state state;
    board_from_string(&state, "********"
                              "********"
                              "********"
                              "********"
                              "****K*r*"
                              "********"
                              "********"
                              "********");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_bishop(void)
{
    Game_state state;
    board_from_string(&state, "********"
                              "********"
                              "******b*"
                              "********"
                              "****K***"
                              "********"
                              "********"
                              "********");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_knight(void)
{
    Game_state state;
    board_from_string(&state, "********"
                              "********"
                              "*****n**"
                              "********"
                              "****K***"
                              "********"
                              "********"
                              "********");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_pawn(void)
{
    Game_state state;
    board_from_string(&state, "********"
                              "********"
                              "********"
                              "*****p**"
                              "****K***"
                              "********"
                              "********"
                              "********");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_king_square_white(void)
{
    Game_state state;
    state.king_white = (Square) { 3, 4 };
    TEST_ASSERT_TRUE(king_square(&state, WHITE)->row == 3
                     && king_square(&state, WHITE)->column == 4);
}

void test_is_king_square_black(void)
{
    Game_state state;
    state.king_black = (Square) { 3, 4 };
    TEST_ASSERT_TRUE(king_square(&state, BLACK)->row == 3
                     && king_square(&state, BLACK)->column == 4);
}

void test_active_player_white(void)
{
    Game_state state;
    state.move_number = 1;
    TEST_ASSERT_TRUE(WHITE == players_turn(&state));
}

void test_active_player_black(void)
{
    Game_state state;
    state.move_number = 2;
    TEST_ASSERT_TRUE(WHITE != players_turn(&state));
}

void test_write_rook_possible_moves(void)
{
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "********"
                              "********"
                              "********"
                              "********"
                              "****R***"
                              "********"
                              "********"
                              "K*******");
    state.king_white = (Square) {0,0};
    Square rook = (Square) {4,3};
    write_rook_possible_moves(&state, &rook);
    TEST_ASSERT_EQUAL_STRING_CHESS("00001000"
                                   "00001000"
                                   "00001000"
                                   "00001000"
                                   "11110111"
                                   "00001000"
                                   "00001000"
                                   "00001000", possible_moves_string(&state, rook));
}

int main(void)
{
    UNITY_BEGIN();

    // core_functions.h
    RUN_TEST(test_board);
    RUN_TEST(test_is_attacked_rook);
    RUN_TEST(test_is_attacked_bishop);
    RUN_TEST(test_is_attacked_knight);
    RUN_TEST(test_is_attacked_pawn);
    RUN_TEST(test_is_king_square_white);
    RUN_TEST(test_is_king_square_black);
    RUN_TEST(test_active_player_white);
    RUN_TEST(test_active_player_black);
    RUN_TEST(test_write_rook_possible_moves);

    return UNITY_END();
}
