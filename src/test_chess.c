/* IMPORTANT:
 * For tests to work #define DEBUG in the following files:
 * core_functions.c
 * core_interface.c
 * core_interface.h
 */

/* options (uncomment to activate) */
//#define TEST_CORE_FUNCTIONS_H
//#define TEST_CORE_INTERFACE_H
#define TEST_DS_LIB_H
#define TEST_TUI_LIB_H

/* include directives */
#include "test-framework/unity/unity.h"
#include "test-framework/unity/unity_chess_extension.h"

#include "chess_test_creator.h"
#include "tui_test_lib.h"
#include "graphic_output.h"
#include "core_functions.h"
#include "core_interface.h"
#include "ds_lib.h"
#include "tui_lib.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

//////////////////////
// core_functions.h //
//////////////////////
#ifdef TEST_CORE_FUNCTIONS_H
void test_board(void)
{
    Game_state state;
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....K..."
                      "........"
                      "........"
                      "........");
    TEST_ASSERT_TRUE(WHITE_i == state.board[3][4].color);
}

void test_is_attacked_by_rook(void)
{
    Game_state state;
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....K.r."
                      "........"
                      "........"
                      "........");
    bool attacked = is_attacked_by(&state, (Square_i) { 3, 4 }, BLACK_i);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_bishop(void)
{
    Game_state state;
    set_board(&state, "........"
                      "........"
                      "......b."
                      "........"
                      "....K..."
                      "........"
                      "........"
                      "........");
    bool attacked = is_attacked_by(&state, (Square_i) { 3, 4 }, BLACK_i);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_knight(void)
{
    Game_state state;
    set_board(&state, "........"
                      "........"
                      ".....n.."
                      "........"
                      "....K..."
                      "........"
                      "........"
                      "........");
    bool attacked = is_attacked_by(&state, (Square_i) { 3, 4 }, BLACK_i);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_pawn(void)
{
    Game_state state;
    set_board(&state, "........"
                      "........"
                      "........"
                      ".....p.."
                      "....K..."
                      "........"
                      "........"
                      "........");
    bool attacked = is_attacked_by(&state, (Square_i) { 3, 4 }, BLACK_i);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_king_square_white(void)
{
    Game_state state;
    state.king_white = (Square_i) { 3, 4 };
    TEST_ASSERT_TRUE(king_square(&state, WHITE_i)->row == 3
                     && king_square(&state, WHITE_i)->column == 4);
}

void test_is_king_square_black(void)
{
    Game_state state;
    state.king_black = (Square_i) { 3, 4 };
    TEST_ASSERT_TRUE(king_square(&state, BLACK_i)->row == 3
                     && king_square(&state, BLACK_i)->column == 4);
}

void test_active_player_white(void)
{
    Game_state state;
    state.move_number = 1;
    TEST_ASSERT_TRUE(WHITE_i == player_active(&state));
}

void test_active_player_black(void)
{
    Game_state state;
    state.move_number = 2;
    TEST_ASSERT_TRUE(WHITE_i != player_active(&state));
}

void test_passive_player_white(void)
{
    Game_state state;
    state.move_number = 2;
    TEST_ASSERT_TRUE(WHITE_i == player_passive(&state));
}

void test_passive_player_black(void)
{
    Game_state state;
    state.move_number = 1;
    TEST_ASSERT_TRUE(WHITE_i != player_passive(&state));
}

void test_set_test_game_state(void)
{
    Game_state state;
    set_test_game_state(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "....0..."
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, (Square_i) {3,4}));
}

void test_is_attacked_by(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "..pPK..."
                      "........"
                      "........"
                      "........");
    state.last_move = (Move_i) {.from = (Square_i) {1,3}, .to = (Square_i) {3,3}};
    state.king_white = (Square_i) {3,4};
    TEST_ASSERT_TRUE(is_attacked_by(&state, (Square_i) {3,3}, BLACK_i));
}

void test_king_square(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....K..."
                      "........"
                      "........"
                      "........");
    state.king_white = (Square_i) {3,4};
    TEST_ASSERT_TRUE((king_square(&state, WHITE_i)->row == state.king_white.row)
                  && (king_square(&state, WHITE_i)->column == state.king_white.column));
}

void test_check_after_move_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....R..."
                      "........"
                      "........"
                      ".K.....k");
    state.king_white = (Square_i) {0,1};
    state.king_black = (Square_i) {0,7};
    //print_board(&state);
    TEST_ASSERT_TRUE(!in_check_after_move(&state, (Move_i) {(Square_i) {3,4}, (Square_i) {2,4}}));
}

void test_check_after_move_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "....r..."
                      "........"
                      "........"
                      "..P.R..."
                      "........"
                      "........"
                      "....K...");
    state.king_white = (Square_i) {0,4};
    Square_i rook = (Square_i) {3,4};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      ".....K.."
                      "....n..."
                      "...P...."
                      "........");
    state.king_white = (Square_i) {3,5};
    Square_i pawn = (Square_i) {1,3};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "........"
                      "...pP..."
                      ".....K.."
                      "........"
                      "........"
                      "........");
    state.last_move = (Move_i) {(Square_i) {6,3}, (Square_i) {4,3}};
    state.king_white = (Square_i) {3,5};
    Square_i pawn = (Square_i) {4,4};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      ".....n.."
                      ".K.pP..r"
                      "........"
                      "........"
                      "........"
                      "........");
    state.last_move = (Move_i) {(Square_i) {6,3}, (Square_i) {4,3}};
    state.king_white = (Square_i) {4,1};
    Square_i pawn = (Square_i) {4,4};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "...K...."
                      ".....n.."
                      "........"
                      "......N."
                      "........"
                      ".....p.."
                      "........");
    state.king_white = (Square_i) {6,3};
    Square_i knight = (Square_i) {3,6};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "...K...."
                      "........"
                      "........"
                      "......N."
                      "........"
                      ".....p.."
                      "........");
    state.king_white = (Square_i) {6,3};
    Square_i knight = (Square_i) {3,6};
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
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "..q...b."
                      "........"
                      "....B..."
                      "........"
                      "........"
                      ".K......");
    state.king_white = (Square_i) {0,1};
    Square_i bishop = (Square_i) {3,4};
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
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....R..."
                      "........"
                      "........"
                      "K.......");
    state.king_white = (Square_i) {0,0};
    Square_i rook = (Square_i) {3,4};
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

void test_write_king_possible_moves_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      ".r......"
                      "........"
                      "........"
                      "Kn......"
                      ".p......"
                      "........"
                      "........");
    state.king_white = (Square_i) {3,0};
    Square_i king = (Square_i) {3,0};
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

void test_write_king_possible_moves_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "..b....."
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    Square_i king = (Square_i) {0,4};
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "...1.1.."
                                   "..110...", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "..b....."
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    Square_i king = (Square_i) {0,4};
    state.castle_qensde_legal_white = false;
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "...1.1.."
                                   "...10...", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "..b....."
                      "........"
                      "........"
                      "....K..R");
    state.king_white = (Square_i) {0,4};
    Square_i king = (Square_i) {0,4};
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "...1.1.."
                                   "...10...", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_05(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;
    set_board(&state, "....k..r"
                      "........"
                      "....R..."
                      "........"
                      "........"
                      "........"
                      "........"
                      "....K...");
    state.king_black = (Square_i) {7,4};
    Square_i king = (Square_i) {7,4};
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("...101.."
                                   "...1.1.."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_06(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;
    set_board(&state, "r...k..r"
                      "........"
                      "...R...."
                      "........"
                      "........"
                      "........"
                      "........"
                      "....K...");
    state.king_black = (Square_i) {7,4};
    Square_i king = (Square_i) {7,4};
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("....011."
                                   "....11.."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_07(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;
    set_board(&state, "r...k..r"
                      "........"
                      "...R...."
                      "........"
                      "........"
                      "........"
                      "........"
                      "....K...");
    state.king_black = (Square_i) {7,4};
    Square_i king = (Square_i) {7,4};
    state.castle_kngsde_legal_black = false;
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("....01.."
                                   "....11.."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_08(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;
    set_board(&state, "rb..k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "....K...");
    state.king_black = (Square_i) {7,4};
    Square_i king = (Square_i) {7,4};
    state.castle_kngsde_legal_black = false;
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("...101.."
                                   "...111.."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_write_king_possible_moves_09(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;
    set_board(&state, ".r..k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "....K...");
    state.king_black = (Square_i) {7,4};
    Square_i king = (Square_i) {7,4};
    state.castle_kngsde_legal_black = false;
    write_king_possible_moves(&state, &king);
    TEST_ASSERT_EQUAL_STRING_CHESS("...101.."
                                   "...111.."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_update_possible_moves_game_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "....r..."
                      "........"
                      "........"
                      "..P.R..."
                      "........"
                      "........"
                      "....K...");
    state.king_white = (Square_i) {0,4};
    Square_i rook = (Square_i) {3,4};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "....1..."
                                   "....1..."
                                   "....1..."
                                   "....0..."
                                   "....1..."
                                   "....1..."
                                   "........", possible_moves_string(&state, rook));
}

void test_update_possible_moves_game_02(void)
{
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      ".....K.."
                      "....n..."
                      "...P...."
                      "........");
    state.king_white = (Square_i) {3,5};
    Square_i pawn = (Square_i) {1,3};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "........"
                                   "...1...."
                                   "...11..."
                                   "...0...."
                                   "........", possible_moves_string(&state, pawn));
}

void test_update_possible_moves_game_03(void)
{
    //TEST_IGNORE();
    // test en passant
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "........"
                      "...pP..."
                      ".....K.."
                      "........"
                      "........"
                      "........");
    state.last_move = (Move_i) {(Square_i) {6,3}, (Square_i) {4,3}};
    state.king_white = (Square_i) {3,5};
    Square_i pawn = (Square_i) {4,4};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "...11..."
                                   "....0..."
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, pawn));
}

void test_update_possible_moves_game_04(void)
{
    //TEST_IGNORE();
    // test en passant: is_check_after_move()
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      ".....n.."
                      ".K.pP..r"
                      "........"
                      "........"
                      "........"
                      "........");
    state.last_move = (Move_i) {(Square_i) {6,3}, (Square_i) {4,3}};
    state.king_white = (Square_i) {4,1};
    Square_i pawn = (Square_i) {4,4};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "....11.."
                                   "....0..."
                                   "........"
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, pawn));
}

void test_update_possible_moves_game_05(void)
{
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "...K...."
                      ".....n.."
                      "........"
                      "......N."
                      "........"
                      ".....p.."
                      "........");
    state.king_white = (Square_i) {6,3};
    Square_i knight = (Square_i) {3,6};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   ".....1.."
                                   "........"
                                   "......0."
                                   "........"
                                   "........"
                                   "........", possible_moves_string(&state, knight));
}

void test_update_possible_moves_game_06(void)
{
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "...K...."
                      "........"
                      "........"
                      "......N."
                      "........"
                      ".....p.."
                      "........");
    state.king_white = (Square_i) {6,3};
    Square_i knight = (Square_i) {3,6};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   ".....1.1"
                                   "....1..."
                                   "......0."
                                   "....1..."
                                   ".....1.1"
                                   "........", possible_moves_string(&state, knight));
}

void test_update_possible_moves_game_07(void)
{
    Game_state state;
    set_test_game_state(&state);
    set_board(&state, "........"
                      "........"
                      "..q...b."
                      "........"
                      "....B..."
                      "........"
                      "........"
                      ".K......");
    state.king_white = (Square_i) {0,1};
    Square_i bishop = (Square_i) {3,4};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "......1."
                                   ".....1.."
                                   "....0..."
                                   "...1...."
                                   "..1....."
                                   "........", possible_moves_string(&state, bishop));
}

void test_update_possible_moves_game_08(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      "........"
                      "........"
                      "........"
                      "....R..."
                      "........"
                      "........"
                      "K.......");
    state.king_white = (Square_i) {0,0};
    Square_i rook = (Square_i) {3,4};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("....1..."
                                   "....1..."
                                   "....1..."
                                   "....1..."
                                   "11110111"
                                   "....1..."
                                   "....1..."
                                   "....1...", possible_moves_string(&state, rook));
}

void test_update_possible_moves_game_09(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      ".r......"
                      "........"
                      "........"
                      "Kn......"
                      ".p......"
                      "........"
                      "........");
    state.king_white = (Square_i) {3,0};
    Square_i king = (Square_i) {3,0};
    update_possible_moves_game(&state);
    TEST_ASSERT_EQUAL_STRING_CHESS("........"
                                   "........"
                                   "........"
                                   "1......."
                                   "0......."
                                   "11......"
                                   "........"
                                   "........", possible_moves_string(&state, king));
}

void test_update_possible_moves_game_10(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "........"
                      ".r......"
                      "........"
                      "........"
                      "Kn......"
                      ".p......"
                      "........"
                      "........");
    state.king_white = (Square_i) {3,0};
    update_possible_moves_game(&state);
    TEST_ASSERT_TRUE(3 == state.possible_moves_number);
}

void test_update_possible_moves_game_11(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B......."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    update_possible_moves_game(&state);
    TEST_ASSERT_TRUE(29 == state.possible_moves_number);
}

void test_board_string(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B....k.."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,5};
    TEST_ASSERT_EQUAL_STRING_CHESS("B....k.."
                                   ".r...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "Q......."
                                   "........", board_string(&state));
}

void test_apply_move_01_move_number(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {1,0}, (Square_i) {1,4}});
    TEST_ASSERT_TRUE(2 == new_state->move_number);
}

void test_apply_move_02_player_active(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {1,0}, (Square_i) {1,4}});
    TEST_ASSERT_TRUE(BLACK_i == player_active(new_state));
}

void test_apply_move_03_board(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {1,0}, (Square_i) {1,4}});
    TEST_ASSERT_EQUAL_STRING_CHESS("B..k...."
                                   ".r...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "....Q..."
                                   "........", board_string(new_state));
}

void test_apply_move_04_uneventful_moves_add(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {1,0}, (Square_i) {1,4}});
    TEST_ASSERT_TRUE(state.uneventful_moves + 1 == new_state->uneventful_moves);
}

void test_apply_move_05_capturing(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,0}, (Square_i) {6,1}});
    TEST_ASSERT_EQUAL_STRING_CHESS("...k...."
                                   ".B...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "Q......."
                                   "........", board_string(new_state));
}

void test_apply_move_06_uneventful_moves_reset(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    state.uneventful_moves = 10;
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,0}, (Square_i) {6,1}});
    TEST_ASSERT_TRUE(0 == new_state->uneventful_moves);
}

void test_apply_move_07_pawn_upgradable(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "B..k...."
                      ".r...P.."
                      "........"
                      "P...B..."
                      "Kn......"
                      ".p......"
                      "Q......."
                      "........");
    state.king_white = (Square_i) {3,0};
    state.king_black = (Square_i) {7,3};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {6,5}, (Square_i) {7,5}});
    TEST_ASSERT_TRUE(true == new_state->pawn_upgradable);
}

void test_apply_move_08_castling_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,4}, (Square_i) {0,6}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r...k..r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R....RK.", board_string(new_state));
}

void test_apply_move_09_castling_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,4}, (Square_i) {0,2}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r...k..r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "..KR...R", board_string(new_state));
}

void test_apply_move_10_castling_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,4}, (Square_i) {7,6}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r....rk."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R...K..R", board_string(new_state));
}

void test_apply_move_11_castling_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,4}, (Square_i) {7,2}});
    TEST_ASSERT_EQUAL_STRING_CHESS("..kr...r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R...K..R", board_string(new_state));
}

void test_apply_move_12_castling_legality_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,4}, (Square_i) {1,4}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_white)
                  && (false == new_state->castle_qensde_legal_white));
}

void test_apply_move_13_castling_legality_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,7}, (Square_i) {7,7}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_white)
                  && (true == new_state->castle_qensde_legal_white));
}

void test_apply_move_14_castling_legality_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,0}, (Square_i) {0,1}});
    TEST_ASSERT_TRUE((true == new_state->castle_kngsde_legal_white)
                  && (false == new_state->castle_qensde_legal_white));
}

void test_apply_move_15_castling_legality_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,4}, (Square_i) {7,5}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_black)
                  && (false == new_state->castle_qensde_legal_black));
}

void test_apply_move_16_castling_legality_05(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,0}, (Square_i) {5,0}});
    TEST_ASSERT_TRUE((true == new_state->castle_kngsde_legal_black)
                  && (false == new_state->castle_qensde_legal_black));
}

void test_apply_move_17_castling_legality_06(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,7}, (Square_i) {7,6}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_black)
                  && (true == new_state->castle_qensde_legal_black));
}

void test_apply_move_18_possible_moves_number(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    state.move_number = 2;     // active_player == BLACK_i
    set_board(&state, "r...k..r"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "........"
                      "R...K..R");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {7,7}, (Square_i) {7,6}});
    TEST_ASSERT_TRUE(25 == new_state->possible_moves_number);

    free(new_state);
}

void test_board_repeated_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "rnbqkbnr"
                      "pppppppp"
                      "........"
                      "........"
                      "........"
                      "........"
                      "PPPPPPPP"
                      "RNBQKBNR");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    TEST_ASSERT_TRUE(1 == state.board_occurences);
}

void test_board_repeated_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "rnbqkbnr"
                      "pppppppp"
                      "........"
                      "........"
                      "........"
                      "........"
                      "PPPPPPPP"
                      "RNBQKBNR");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    Game_state *new_state = apply_move(&state, (Move_i) {(Square_i) {0,1}, (Square_i) {2,0}});
    TEST_ASSERT_TRUE(1 == new_state->board_occurences);

    free(new_state);
}

void test_board_repeated_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "rnbqkbnr"
                      "pppppppp"
                      "........"
                      "........"
                      "........"
                      "........"
                      "PPPPPPPP"
                      "RNBQKBNR");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    update_possible_moves_game(&state);
    //printf("move_number = %d\npossible_moves_number = %d\n", state.move_number, state.possible_moves_number);
    Game_state *new_state_1 = apply_move(&state, (Move_i) {(Square_i) {0,1}, (Square_i) {2,0}});
    Game_state *new_state_2 = apply_move(new_state_1, (Move_i) {(Square_i) {7,1}, (Square_i) {5,0}});
    Game_state *new_state_3 = apply_move(new_state_2, (Move_i) {(Square_i) {2,0}, (Square_i) {0,1}});
    Game_state *new_state_4 = apply_move(new_state_3, (Move_i) {(Square_i) {5,0}, (Square_i) {7,1}});
    //printf("move_number = %d\npossible_moves_number = %d\n", new_state_4->move_number, new_state_4->possible_moves_number);
    TEST_ASSERT_TRUE(2 == new_state_4->board_occurences);

    free(new_state_4);
    free(new_state_3);
    free(new_state_2);
    free(new_state_1);
}

void test_board_repeated_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE_i
    set_board(&state, "rnbqkbnr"
                      "pppppppp"
                      "........"
                      "........"
                      "........"
                      "........"
                      "PPPPPPPP"
                      "RNBQKBNR");
    state.king_white = (Square_i) {0,4};
    state.king_black = (Square_i) {7,4};
    update_possible_moves_game(&state);
    //printf("move_number = %d\npossible_moves_number = %d\n", state.move_number, state.possible_moves_number);
    Game_state *new_state_1 = apply_move(&state, (Move_i) {(Square_i) {0,1}, (Square_i) {2,0}});
    Game_state *new_state_2 = apply_move(new_state_1, (Move_i) {(Square_i) {7,1}, (Square_i) {5,0}});
    Game_state *new_state_3 = apply_move(new_state_2, (Move_i) {(Square_i) {2,0}, (Square_i) {0,1}});
    Game_state *new_state_4 = apply_move(new_state_3, (Move_i) {(Square_i) {5,0}, (Square_i) {7,1}});
    Game_state *new_state_5 = apply_move(new_state_4, (Move_i) {(Square_i) {1,4}, (Square_i) {3,4}});
    //printf("move_number = %d\npossible_moves_number = %d\n", new_state_4->move_number, new_state_4->possible_moves_number);
    TEST_ASSERT_TRUE(1 == new_state_5->board_occurences);

    free(new_state_5);
    free(new_state_4);
    free(new_state_3);
    free(new_state_2);
    free(new_state_1);
}
#endif

//////////////////////
// core_interface.h //
//////////////////////
#ifdef TEST_CORE_INTERFACE_H
void test_move_piece_01(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };

    TEST_ASSERT_TRUE(move_piece(game, move));

    destroy_game(game);
}

void test_move_piece_02(void)
{
    Game game = create_game();
    Move move = { (Square) {0,2}, (Square) {1,1} };

    TEST_ASSERT_TRUE(!move_piece(game, move));

    destroy_game(game);
}

void test_move_piece_03(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(game, move);

    TEST_ASSERT_TRUE(WHITE_i == access_state(game)->board[3][3].color);

    destroy_game(game);
}

void test_move_piece_04(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(game, move);

    TEST_ASSERT_TRUE(NONE_i == access_state(game)->board[1][3].color);

    destroy_game(game);
}

void test_move_piece_05(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(game, move);

    TEST_ASSERT_TRUE(PAWN == access_state(game)->board[3][3].kind);

    destroy_game(game);
}

void test_move_piece_06(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(game, move);

    TEST_ASSERT_TRUE(EMPTY == access_state(game)->board[1][3].kind);

    destroy_game(game);
}

void test_player_to_move_01(void)
{
    Game game = create_game();

    TEST_ASSERT_TRUE(WHITE == player_to_move(game));

    destroy_game(game);
}

void test_player_to_move_02(void)
{
    Game game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(game, move);

    TEST_ASSERT_TRUE(BLACK == player_to_move(game));

    destroy_game(game);
}

void test_current_board_01(void)
{
    Game game = create_game();

    TEST_ASSERT_EQUAL_STRING_CHESS("rnbqkbnr"
                                   "pppppppp"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "PPPPPPPP"
                                   "RNBQKBNR", current_board(game));

    destroy_game(game);
}

void test_current_board_02(void)
{
    Game game = create_game();
    Move move = { (Square) {0,1}, (Square) {2,2} };
    move_piece(game, move);

    TEST_ASSERT_EQUAL_STRING_CHESS("rnbqkbnr"
                                   "pppppppp"
                                   "........"
                                   "........"
                                   "........"
                                   "..N....."
                                   "PPPPPPPP"
                                   "R.BQKBNR", current_board(game));

    destroy_game(game);
}

void test_claim_remis_move_01(void)
{
    Game game = create_game();
    move_piece(game, (Move) { (Square) {0,1}, (Square) {2,2} });
    move_piece(game, (Move) { (Square) {7,1}, (Square) {5,2} });
    move_piece(game, (Move) { (Square) {2,2}, (Square) {0,1} });
    move_piece(game, (Move) { (Square) {5,2}, (Square) {7,1} });
    move_piece(game, (Move) { (Square) {0,1}, (Square) {2,2} });
    move_piece(game, (Move) { (Square) {7,1}, (Square) {5,2} });
    move_piece(game, (Move) { (Square) {2,2}, (Square) {0,1} });

    TEST_ASSERT_TRUE(claim_remis_move(game, (Move) { (Square) {5,2}, (Square) {7,1} }));

    destroy_game(game);
}

void test_claim_remis_move_02(void)
{
    Game game = create_game();
    move_piece(game, (Move) { (Square) {0,1}, (Square) {2,2} });
    move_piece(game, (Move) { (Square) {7,1}, (Square) {5,2} });
    move_piece(game, (Move) { (Square) {2,2}, (Square) {0,1} });
    move_piece(game, (Move) { (Square) {5,2}, (Square) {7,1} });
    move_piece(game, (Move) { (Square) {0,1}, (Square) {2,2} });

    TEST_ASSERT_TRUE(!claim_remis_move(game, (Move) { (Square) {7,1}, (Square) {5,2} }));

    destroy_game(game);
}

void test_duplicate_game_01(void)
{
    Game original_game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(original_game, move);

    Game game_duplicate = duplicate_game(original_game);

    TEST_ASSERT_EQUAL_STRING_CHESS(board_string(access_state(original_game)), board_string(access_state(game_duplicate)));

    destroy_game(game_duplicate);
    destroy_game(original_game);
}

void test_duplicate_game_02(void)
{
    Game original_game = create_game();
    Move move = { (Square) {1,3}, (Square) {3,3} };
    move_piece(original_game, move);

    Game game_duplicate = duplicate_game(original_game);

    TEST_ASSERT_TRUE(access_state(original_game)->possible_moves_number == access_state(game_duplicate)->possible_moves_number);

    destroy_game(game_duplicate);
    destroy_game(original_game);
}

void test_pawn_upgradable_01(void)
{
    Game game = create_game();
    access_state(game)->move_number = 1;
    set_game_state(access_state(game), "k......."
                                       "....P..."
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "K.......");
    move_piece(game, (Move) {(Square) {6,4}, (Square) {7,4}});
    TEST_ASSERT_TRUE((7 == pawn_upgradable(game).row) && (4 == pawn_upgradable(game).column));

    free(game);
}

void test_pawn_upgradable_02(void)
{
    Game game = create_game();
    access_state(game)->move_number = 1;
    set_game_state(access_state(game), "k......."
                                       "....P..."
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "K.......");
    move_piece(game, (Move) {(Square) {6,4}, (Square) {7,4}});
    move_piece(game, (Move) {(Square) {7,0}, (Square) {6,0}});
    move_piece(game, (Move) {(Square) {0,0}, (Square) {1,0}});
    TEST_ASSERT_TRUE((-1 == pawn_upgradable(game).row) && (-1 == pawn_upgradable(game).column));

    free(game);
}

void test_upgrade_pawn_01(void)
{
    Game game = create_game();
    access_state(game)->move_number = 1;
    set_game_state(access_state(game), "k......."
                                       "....P..."
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "........"
                                       "K.......");
    move_piece(game, (Move) {(Square) {6,4}, (Square) {7,4}});
    upgrade_pawn(game, 'Q');
    TEST_ASSERT_EQUAL_STRING_CHESS("k...Q..."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "K.......", board_string(access_state(game)));

    free(game);
}
#endif

//////////////
// ds_lib.h //
//////////////
#ifdef TEST_DS_LIB_H
void test_stack_int_is_empty_01(void)
{
    Stack_int stack = stack_int_create();
    TEST_ASSERT_TRUE(stack_int_is_empty(stack));
    stack_int_destroy(stack);
}

void test_stack_int_is_empty_02(void)
{
    Stack_int stack = stack_int_create();
    stack_int_push(stack, 5);
    TEST_ASSERT_TRUE(!stack_int_is_empty(stack));
    stack_int_destroy(stack);
}

void test_stack_int_is_empty_03(void)
{
    Stack_int stack = stack_int_create();
    stack_int_push(stack, 5);
    int n;
    stack_int_pop(stack, &n);
    TEST_ASSERT_TRUE(stack_int_is_empty(stack));
    stack_int_destroy(stack);
}

void test_stack_int_peek_01(void)
{
    Stack_int stack = stack_int_create();
    int n;
    TEST_ASSERT_TRUE(!stack_int_peek(stack, &n));
    stack_int_destroy(stack);
}

void test_stack_int_peek_02(void)
{
    Stack_int stack = stack_int_create();
    stack_int_push(stack, 5);
    int n;
    stack_int_peek(stack, &n);
    TEST_ASSERT_TRUE(5 == n);
    stack_int_destroy(stack);
}

void test_stack_int_pop_01(void)
{
    Stack_int stack = stack_int_create();
    int n;
    TEST_ASSERT_TRUE(!stack_int_pop(stack, &n));
    stack_int_destroy(stack);
}

void test_stack_int_pop_02(void)
{
    Stack_int stack = stack_int_create();
    stack_int_push(stack, 5);
    int n;
    stack_int_pop(stack, &n);
    TEST_ASSERT_TRUE(5 == n);
    stack_int_destroy(stack);
}
#endif

#ifdef TEST_TUI_LIB_H
// tests here got accidentally deleted
void test_window_create_01(void)
{
    Window w1 = window_create();

    int amount;
    int *ids_w1 = window_screens_id(w1, &amount);

    TEST_ASSERT_TRUE(0 == amount);

    window_destroy(w1);

    free(ids_w1);
}

void test_screen_create_01(void)
{
    Screen s1 = screen_create();

    int amount;
    int *ids_s1 = screen_windows_id(s1, &amount);

    TEST_ASSERT_TRUE(0 == amount);

    screen_destroy(s1);

    free(ids_s1);
}

void test_screen_add_window_01_one_screen_one_window_01(void)
{
    Window w1 = window_create();
    Screen s1 = screen_create();

    screen_add_window(s1, w1, 1);

    int prios_s1_ought[1] = {1};
    int ids_s1_ought[1] = {1};
    int ids_w1_ought[1] = {2};

    int amount;
    int *prios_s1 = screen_windows_prio(s1, &amount);
    int *ids_s1 = screen_windows_id(s1, &amount);
    int *ids_w1 = window_screens_id(w1, &amount);

    TEST_ASSERT_TRUE((compare_arrays_int(prios_s1, prios_s1_ought, 1))
                  && (compare_arrays_int(ids_s1, ids_s1_ought, 1))
                  && (compare_arrays_int(ids_w1, ids_w1_ought, 1)));

    screen_destroy(s1);
    window_destroy(w1);

    free(prios_s1);
    free(ids_s1);
    free(ids_w1);
}

void test_screen_add_window_11_multiple_screens_multiple_windows_01(void)
{
    Screen s1 = screen_create();
    Screen s2 = screen_create();
    Window w1 = window_create();
    Window w2 = window_create();

    screen_add_window(s1, w1, 1);
    screen_add_window(s2, w2, 1);
    screen_add_window(s2, w1, 1);
    screen_add_window(s1, w2, 1);

    /*
    screen_print_info(s1);
    screen_print_info(s2);
    window_print_info(w1);
    window_print_info(w2);
    */

    int prios_s1_ought[2] = {0,1};
    int prios_s2_ought[2] = {0,1};
    int ids_s1_ought[2] = {3,4};
    int ids_s2_ought[2] = {4,3};
    int ids_w1_ought[2] = {2,1};
    int ids_w2_ought[2] = {1,2};

    int amount;
    int *prios_s1 = screen_windows_prio(s1, &amount);
    int *prios_s2 = screen_windows_prio(s2, &amount);
    int *ids_s1 = screen_windows_id(s1, &amount);
    int *ids_s2 = screen_windows_id(s2, &amount);
    int *ids_w1 = window_screens_id(w1, &amount);
    int *ids_w2 = window_screens_id(w2, &amount);

    TEST_ASSERT_TRUE((compare_arrays_int(prios_s1, prios_s1_ought, 2))
                  && (compare_arrays_int(prios_s2, prios_s2_ought, 2))
                  && (compare_arrays_int(ids_s1, ids_s1_ought, 2))
                  && (compare_arrays_int(ids_s2, ids_s2_ought, 2))
                  && (compare_arrays_int(ids_w1, ids_w1_ought, 2))
                  && (compare_arrays_int(ids_w2, ids_w2_ought, 2)));

    window_destroy(w2);
    window_destroy(w1);
    screen_destroy(s2);
    screen_destroy(s1);

    free(prios_s1);
    free(prios_s2);
    free(ids_s1);
    free(ids_s2);
    free(ids_w1);
    free(ids_w2);
}

void test_screen_add_window_12_multiple_screens_multiple_windows_02(void)
{
    Screen s1 = screen_create();
    Screen s2 = screen_create();
    Window w1 = window_create();
    Window w2 = window_create();

    screen_add_window(s1, w1, 1);
    screen_add_window(s2, w2, 1);
    screen_add_window(s2, w1, 1);
    screen_add_window(s1, w2, 1);

    window_destroy(w2);

    int prios_s1_ought[1] = {0};
    int prios_s2_ought[1] = {1};
    int ids_s1_ought[1] = {3};
    int ids_s2_ought[1] = {3};
    int ids_w1_ought[2] = {2,1};

    int amount;
    int *prios_s1 = screen_windows_prio(s1, &amount);
    int *prios_s2 = screen_windows_prio(s2, &amount);
    int *ids_s1 = screen_windows_id(s1, &amount);
    int *ids_s2 = screen_windows_id(s2, &amount);
    int *ids_w1 = window_screens_id(w1, &amount);

    TEST_ASSERT_TRUE((compare_arrays_int(prios_s1, prios_s1_ought, 1))
                  && (compare_arrays_int(prios_s2, prios_s2_ought, 1))
                  && (compare_arrays_int(ids_s1, ids_s1_ought, 1))
                  && (compare_arrays_int(ids_s2, ids_s2_ought, 1))
                  && (compare_arrays_int(ids_w1, ids_w1_ought, 2)));

    window_destroy(w1);
    screen_destroy(s2);
    screen_destroy(s1);

    free(prios_s1);
    free(prios_s2);
    free(ids_s1);
    free(ids_s2);
    free(ids_w1);
}

void test_screen_add_window_13_multiple_screens_multiple_windows_03(void)
{
    Window w1 = window_create();
    Window w2 = window_create();
    Screen s1 = screen_create();
    Screen s2 = screen_create();

    screen_add_window(s1, w1, 1);
    screen_add_window(s2, w2, 1);
    screen_add_window(s2, w1, 1);
    screen_add_window(s1, w2, 1);

    screen_destroy(s2);

    int prios_s1_ought[2] = {0,1};
    int ids_s1_ought[2] = {1,2};
    int ids_w1_ought[2] = {3};
    int ids_w2_ought[2] = {3};

    int amount;
    int *prios_s1 = screen_windows_prio(s1, &amount);
    int *ids_s1 = screen_windows_id(s1, &amount);
    int *ids_w1 = window_screens_id(w1, &amount);
    int *ids_w2 = window_screens_id(w2, &amount);

    TEST_ASSERT_TRUE((compare_arrays_int(prios_s1, prios_s1_ought, 2))
                  && (compare_arrays_int(ids_s1, ids_s1_ought, 2))
                  && (compare_arrays_int(ids_w1, ids_w1_ought, 1))
                  && (compare_arrays_int(ids_w2, ids_w2_ought, 1)));

    screen_destroy(s1);
    window_destroy(w2);
    window_destroy(w1);

    free(prios_s1);
    free(ids_s1);
    free(ids_w1);
    free(ids_w2);
}

void test_screen_remove_window_01(void)
{
    Screen s1 = screen_create();
    Window w1 = window_create();

    screen_add_window(s1, w1, 1);

    screen_remove_window(s1, w1);

    int amount_s1;
    int *prios_s1 = screen_windows_prio(s1, &amount_s1);
    int amount_w1;
    int *ids_w1 = window_screens_id(w1, &amount_w1);

    TEST_ASSERT_TRUE((0 == amount_s1) && (0 == amount_s1));

    window_destroy(w1);
    screen_destroy(s1);

    free(prios_s1);
    free(ids_w1);
}

void test_screen_remove_window_02(void)
{
    Screen s1 = screen_create();
    Screen s2 = screen_create();
    Window w1 = window_create();
    Window w2 = window_create();

    screen_add_window(s1, w1, 1);
    screen_add_window(s2, w2, 1);
    screen_add_window(s2, w1, 1);
    screen_add_window(s1, w2, 1);

    screen_remove_window(s1, w1);

    int prios_s1_ought[2] = {1};
    int prios_s2_ought[2] = {0,1};
    int ids_s1_ought[2] = {4};
    int ids_s2_ought[2] = {4,3};
    int ids_w1_ought[2] = {2};
    int ids_w2_ought[2] = {1,2};

    int amount;
    int *prios_s1 = screen_windows_prio(s1, &amount);
    int *prios_s2 = screen_windows_prio(s2, &amount);
    int *ids_s1 = screen_windows_id(s1, &amount);
    int *ids_s2 = screen_windows_id(s2, &amount);
    int *ids_w1 = window_screens_id(w1, &amount);
    int *ids_w2 = window_screens_id(w2, &amount);

    TEST_ASSERT_TRUE((compare_arrays_int(prios_s1, prios_s1_ought, 1))
                  && (compare_arrays_int(prios_s2, prios_s2_ought, 2))
                  && (compare_arrays_int(ids_s1, ids_s1_ought, 1))
                  && (compare_arrays_int(ids_s2, ids_s2_ought, 2))
                  && (compare_arrays_int(ids_w1, ids_w1_ought, 1))
                  && (compare_arrays_int(ids_w2, ids_w2_ought, 2)));

    window_destroy(w2);
    window_destroy(w1);
    screen_destroy(s2);
    screen_destroy(s1);

    free(prios_s1);
    free(prios_s2);
    free(ids_s1);
    free(ids_s2);
    free(ids_w1);
    free(ids_w2);
}

void test_screen_remove_window_03(void)
{
    Screen s1 = screen_create();
    Screen s2 = screen_create();
    Screen s3 = screen_create();
    Screen s4 = screen_create();
    Screen s5 = screen_create();
    Window w1 = window_create();

    screen_add_window(s1, w1, 1);
    screen_add_window(s2, w1, 1);
    screen_add_window(s3, w1, 1);
    screen_add_window(s4, w1, 1);
    screen_add_window(s5, w1, 1);

    screen_remove_window(s1, w1);

    int amount_w1;
    int *ids_w1 = window_screens_id(w1, &amount_w1);
    int ids_w1_ought[4] = {5,4,3,2};

    TEST_ASSERT_TRUE(compare_arrays_int(ids_w1, ids_w1_ought, 4) && (4 == amount_w1));

    window_destroy(w1);
    screen_destroy(s5);
    screen_destroy(s4);
    screen_destroy(s3);
    screen_destroy(s2);
    screen_destroy(s1);

    free(ids_w1);
}

void test_window_print_01(void)
{
    Window w = window_create();
    char *str = "Hello my name is this and that, and other things\nNow we are talking!\n";
    window_update_content(w, str, strlen(str));
    window_set_frame(w, '=', '|', 'O');
    window_display_frame(w, true);
    window_set_space(w, 1, 1, 2, 2);
    window_set_size(w, 20, 40);
    window_print(w, ' ', ' ');
    TEST_ASSERT_TRUE(false);
    window_destroy(w);
}

#endif

int main(void)
{
    UNITY_BEGIN();

    // core_functions.h
    #ifdef TEST_CORE_FUNCTIONS_H
    RUN_TEST(test_board);
    RUN_TEST(test_is_attacked_by_rook);
    RUN_TEST(test_is_attacked_by_bishop);
    RUN_TEST(test_is_attacked_by_knight);
    RUN_TEST(test_is_attacked_by_pawn);
    RUN_TEST(test_is_king_square_white);
    RUN_TEST(test_is_king_square_black);
    RUN_TEST(test_active_player_white);
    RUN_TEST(test_active_player_black);
    RUN_TEST(test_passive_player_white);
    RUN_TEST(test_passive_player_black);
    RUN_TEST(test_set_test_game_state);
    RUN_TEST(test_is_attacked_by);
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
    RUN_TEST(test_write_king_possible_moves_01);
    RUN_TEST(test_write_king_possible_moves_02);
    RUN_TEST(test_write_king_possible_moves_03);
    RUN_TEST(test_write_king_possible_moves_04);
    RUN_TEST(test_write_king_possible_moves_05);
    RUN_TEST(test_write_king_possible_moves_06);
    RUN_TEST(test_write_king_possible_moves_07);
    RUN_TEST(test_write_king_possible_moves_08);
    RUN_TEST(test_write_king_possible_moves_09);
    RUN_TEST(test_update_possible_moves_game_01);
    RUN_TEST(test_update_possible_moves_game_02);
    RUN_TEST(test_update_possible_moves_game_03);
    RUN_TEST(test_update_possible_moves_game_04);
    RUN_TEST(test_update_possible_moves_game_05);
    RUN_TEST(test_update_possible_moves_game_06);
    RUN_TEST(test_update_possible_moves_game_07);
    RUN_TEST(test_update_possible_moves_game_08);
    RUN_TEST(test_update_possible_moves_game_09);
    RUN_TEST(test_update_possible_moves_game_10);
    RUN_TEST(test_update_possible_moves_game_11);
    RUN_TEST(test_board_string);
    RUN_TEST(test_apply_move_01_move_number);
    RUN_TEST(test_apply_move_02_player_active);
    RUN_TEST(test_apply_move_03_board);
    RUN_TEST(test_apply_move_04_uneventful_moves_add);
    RUN_TEST(test_apply_move_05_capturing);
    RUN_TEST(test_apply_move_06_uneventful_moves_reset);
    RUN_TEST(test_apply_move_07_pawn_upgradable);
    RUN_TEST(test_apply_move_08_castling_01);
    RUN_TEST(test_apply_move_09_castling_02);
    RUN_TEST(test_apply_move_10_castling_03);
    RUN_TEST(test_apply_move_11_castling_04);
    RUN_TEST(test_apply_move_12_castling_legality_01);
    RUN_TEST(test_apply_move_13_castling_legality_02);
    RUN_TEST(test_apply_move_14_castling_legality_03);
    RUN_TEST(test_apply_move_15_castling_legality_04);
    RUN_TEST(test_apply_move_16_castling_legality_05);
    RUN_TEST(test_apply_move_17_castling_legality_06);
    RUN_TEST(test_apply_move_18_possible_moves_number);
    RUN_TEST(test_board_repeated_01);
    RUN_TEST(test_board_repeated_02);
    RUN_TEST(test_board_repeated_03);
    RUN_TEST(test_board_repeated_04);
    #endif

    // core_interface.h
    #ifdef TEST_CORE_INTERFACE_H
    RUN_TEST(test_move_piece_01);
    RUN_TEST(test_move_piece_02);
    RUN_TEST(test_move_piece_03);
    RUN_TEST(test_move_piece_04);
    RUN_TEST(test_move_piece_05);
    RUN_TEST(test_move_piece_06);
    RUN_TEST(test_player_to_move_01);
    RUN_TEST(test_player_to_move_02);
    RUN_TEST(test_current_board_01);
    RUN_TEST(test_current_board_02);
    RUN_TEST(test_claim_remis_move_01);
    RUN_TEST(test_claim_remis_move_02);
    RUN_TEST(test_duplicate_game_01);
    RUN_TEST(test_duplicate_game_02);
    RUN_TEST(test_pawn_upgradable_01);
    RUN_TEST(test_pawn_upgradable_02);
    RUN_TEST(test_upgrade_pawn_01);
    #endif

    #ifdef TEST_DS_LIB_H
    RUN_TEST(test_stack_int_is_empty_01);
    RUN_TEST(test_stack_int_is_empty_02);
    RUN_TEST(test_stack_int_is_empty_03);
    RUN_TEST(test_stack_int_peek_01);
    RUN_TEST(test_stack_int_peek_02);
    RUN_TEST(test_stack_int_pop_01);
    RUN_TEST(test_stack_int_pop_02);
    #endif

    #ifdef TEST_TUI_LIB_H
    // tests here got accidentally deleted
    RUN_TEST(test_window_create_01);
    RUN_TEST(test_screen_create_01);
    RUN_TEST(test_screen_add_window_01_one_screen_one_window_01);
    RUN_TEST(test_screen_add_window_11_multiple_screens_multiple_windows_01);
    RUN_TEST(test_screen_add_window_12_multiple_screens_multiple_windows_02);
    RUN_TEST(test_screen_add_window_13_multiple_screens_multiple_windows_03);
    RUN_TEST(test_screen_remove_window_01);
    RUN_TEST(test_screen_remove_window_02);
    RUN_TEST(test_screen_remove_window_03);
    RUN_TEST(test_window_print_01);
    #endif

    return UNITY_END();
}
