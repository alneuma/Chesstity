#include "test-framework/unity/unity.h"
#include "test-framework/unity/unity_chess_extension.h"

#include "chess_test_creator.h"
#include "graphic_output.h"
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
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "....K..."
                              "........"
                              "........"
                              "........");
    TEST_ASSERT_TRUE(WHITE == state.board[3][4].color);
}

void test_is_attacked_rook(void)
{
    Game_state state;
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "....K.r."
                              "........"
                              "........"
                              "........");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_bishop(void)
{
    Game_state state;
    board_from_string(&state, "........"
                              "........"
                              "......b."
                              "........"
                              "....K..."
                              "........"
                              "........"
                              "........");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_knight(void)
{
    Game_state state;
    board_from_string(&state, "........"
                              "........"
                              ".....n.."
                              "........"
                              "....K..."
                              "........"
                              "........"
                              "........");
    bool attacked = is_attacked(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_pawn(void)
{
    Game_state state;
    board_from_string(&state, "........"
                              "........"
                              "........"
                              ".....p.."
                              "....K..."
                              "........"
                              "........"
                              "........");
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

void test_set_game_state(void)
{
    Game_state state;
    set_game_state(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "....0..."
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, (Square) {3,4}));
}

void test_is_attacked(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "..pPK..."
                              "........"
                              "........"
                              "........");
    state.last_move = (Move) {.from = (Square) {1,3}, .to = (Square) {3,3}};
    state.king_white = (Square) {3,4};
    TEST_ASSERT_TRUE(is_attacked(&state, (Square) {3,3}, BLACK));
}

void test_king_square(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "....K..."
                              "........"
                              "........"
                              "........");
    state.king_white = (Square) {3,4};
    TEST_ASSERT_TRUE((king_square(&state, WHITE)->row == state.king_white.row)
                  && (king_square(&state, WHITE)->column == state.king_white.column));
}

void test_check_after_move_01(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "....R..."
                              "........"
                              "........"
                              ".K.....k");
    state.king_white = (Square) {0,1};
    state.king_black = (Square) {0,7};
    //print_board(&state);
    TEST_ASSERT_TRUE(!in_check_after_move(&state, (Move) {(Square) {3,4}, (Square) {2,4}}));
}

void test_check_after_move_02(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "....r..."
                              "........"
                              "........"
                              "..P.R..."
                              "........"
                              "........"
                              "....K...");
    state.king_white = (Square) {0,4};
    Square rook = (Square) {3,4};
    write_rook_possible_moves(&state, &rook);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "....1..."
                                   "....1..."
                                   "....1..."
                                   "....0..."
                                   "....1..."
                                   "....1..."
                                   "........", possible_moves_string(&state, rook));
}

void test_write_pawn_possible_moves_01(void)
{
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              ".....K.."
                              "....n..."
                              "...P...."
                              "........");
    state.king_white = (Square) {3,5};
    Square pawn = (Square) {1,3};
    write_pawn_possible_moves(&state, &pawn);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "...1...."
                                   "...11..."
                                   "...0...."
                                   "........", possible_moves_string(&state, pawn));
}

void test_write_pawn_possible_moves_02(void)
{
    //TEST_IGNORE();
    // test en passant
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "...pP..."
                              ".....K.."
                              "........"
                              "........"
                              "........");
    state.last_move = (Move) {(Square) {6,3}, (Square) {4,3}};
    state.king_white = (Square) {3,5};
    Square pawn = (Square) {4,4};
    write_pawn_possible_moves(&state, &pawn);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "...11..."
                                   "....0..."
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, pawn));
}

void test_write_pawn_possible_moves_03(void)
{
    //TEST_IGNORE();
    // test en passant: is_check_after_move()
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "........"
                              ".....n.."
                              ".K.pP..r"
                              "........"
                              "........"
                              "........"
                              "........");
    state.last_move = (Move) {(Square) {6,3}, (Square) {4,3}};
    state.king_white = (Square) {4,1};
    Square pawn = (Square) {4,4};
    write_pawn_possible_moves(&state, &pawn);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "....11.."
                                   "....0..."
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, pawn));
}

void test_write_knight_possible_moves_01(void)
{
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "...K...."
                              ".....n.."
                              "........"
                              "......N."
                              "........"
                              ".....p.."
                              "........");
    state.king_white = (Square) {6,3};
    Square knight = (Square) {3,6};
    write_knight_possible_moves(&state, &knight);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   ".....1.."
                                   "........"
                                   "......0."
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, knight));
}

void test_write_knight_possible_moves_02(void)
{
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "...K...."
                              "........"
                              "........"
                              "......N."
                              "........"
                              ".....p.."
                              "........");
    state.king_white = (Square) {6,3};
    Square knight = (Square) {3,6};
    write_knight_possible_moves(&state, &knight);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   ".....1.1"
                                   "....1..."
                                   "......0."
                                   "....1..."
                                   ".....1.1"
                                   "........", possible_moves_string(&state, knight));
}

void test_write_bishop_possible_moves(void)
{
    Game_state state;
    set_game_state(&state);
    board_from_string(&state, "........"
                              "........"
                              "..q...b."
                              "........"
                              "....B..."
                              "........"
                              "........"
                              ".K......");
    state.king_white = (Square) {0,1};
    Square bishop = (Square) {3,4};
    write_bishop_possible_moves(&state, &bishop);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "......1."
                                   ".....1.."
                                   "....0..."
                                   "...1...."
                                   "..1....."
                                   "........", possible_moves_string(&state, bishop));
}

void test_write_rook_possible_moves(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "....R..."
                              "........"
                              "........"
                              "K.......");
    state.king_white = (Square) {0,0};
    Square rook = (Square) {3,4};
    write_rook_possible_moves(&state, &rook);
    TEST_ASSERT_EQUAL_STRING_CHESS("....1..."
                                   "....1..."
                                   "....1..."
                                   "....1..."
                                   "11110111"
                                   "....1..."
                                   "....1..."
                                   "....1...", possible_moves_string(&state, rook));
}

void test_write_king_possible_moves(void)
{
    Game_state state;
    set_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              ".r......"
                              "........"
                              "........"
                              "Kn......"
                              ".p......"
                              "........"
                              "........");
    state.king_white = (Square) {3,0};
    Square king = (Square) {3,0};
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "1......."
                                   "0......."
                                   "11......"
                                   "........"
                                   "........", possible_moves_string(&state, king));
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
    RUN_TEST(test_set_game_state);
    RUN_TEST(test_is_attacked);
    RUN_TEST(test_king_square);
    RUN_TEST(test_check_after_move_01);
    RUN_TEST(test_check_after_move_02);
    RUN_TEST(test_write_pawn_possible_moves_01);
    RUN_TEST(test_write_pawn_possible_moves_02);
    RUN_TEST(test_write_pawn_possible_moves_03);
    RUN_TEST(test_write_knight_possible_moves_01);
    RUN_TEST(test_write_knight_possible_moves_02);
    RUN_TEST(test_write_bishop_possible_moves);
    RUN_TEST(test_write_rook_possible_moves);
    RUN_TEST(test_write_king_possible_moves);

    return UNITY_END();
}
