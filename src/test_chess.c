#include "test-framework/unity/unity.h"
#include "test-framework/unity/unity_chess_extension.h"

#include "chess_test_creator.h"
#include "graphic_output.h"
#include "core_interface.h"
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

void test_is_attacked_by_rook(void)
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
    bool attacked = is_attacked_by(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_bishop(void)
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
    bool attacked = is_attacked_by(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_knight(void)
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
    bool attacked = is_attacked_by(&state, (Square) { 3, 4 }, BLACK);
    TEST_ASSERT_TRUE(attacked);
}

void test_is_attacked_by_pawn(void)
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
    bool attacked = is_attacked_by(&state, (Square) { 3, 4 }, BLACK);
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
    TEST_ASSERT_TRUE(WHITE == player_active(&state));
}

void test_active_player_black(void)
{
    Game_state state;
    state.move_number = 2;
    TEST_ASSERT_TRUE(WHITE != player_active(&state));
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
                                   "........", possible_moves_string(&state, (Square) {3,4}));
}

void test_is_attacked_by(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
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
    TEST_ASSERT_TRUE(is_attacked_by(&state, (Square) {3,3}, BLACK));
}

void test_king_square(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);
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
    set_test_game_state(&state);    // active_player == WHITE
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

void test_write_king_possible_moves_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
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

void test_write_king_possible_moves_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "..b....."
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    Square king = (Square) {0,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "..b....."
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    Square king = (Square) {0,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              "........"
                              "........"
                              "........"
                              "..b....."
                              "........"
                              "........"
                              "....K..R");
    state.king_white = (Square) {0,4};
    Square king = (Square) {0,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;
    board_from_string(&state, "....k..r"
                              "........"
                              "....R..."
                              "........"
                              "........"
                              "........"
                              "........"
                              "....K...");
    state.king_black = (Square) {7,4};
    Square king = (Square) {7,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;
    board_from_string(&state, "r...k..r"
                              "........"
                              "...R...."
                              "........"
                              "........"
                              "........"
                              "........"
                              "....K...");
    state.king_black = (Square) {7,4};
    Square king = (Square) {7,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;
    board_from_string(&state, "r...k..r"
                              "........"
                              "...R...."
                              "........"
                              "........"
                              "........"
                              "........"
                              "....K...");
    state.king_black = (Square) {7,4};
    Square king = (Square) {7,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;
    board_from_string(&state, "rb..k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "....K...");
    state.king_black = (Square) {7,4};
    Square king = (Square) {7,4};
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
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;
    board_from_string(&state, ".r..k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "....K...");
    state.king_black = (Square) {7,4};
    Square king = (Square) {7,4};
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
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);    // active_player == WHITE
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
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "........"
                              ".r......"
                              "........"
                              "........"
                              "Kn......"
                              ".p......"
                              "........"
                              "........");
    state.king_white = (Square) {3,0};
    update_possible_moves_game(&state);
    TEST_ASSERT_TRUE(3 == state.possible_moves_number);
}

void test_update_possible_moves_game_11(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B......."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    update_possible_moves_game(&state);
    TEST_ASSERT_TRUE(29 == state.possible_moves_number);
}

void test_board_string(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B....k.."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,5};
    TEST_ASSERT_EQUAL_STRING_CHESS("B....k.."
                                   ".r...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "Q......."
                                   "........", board_string(&state));
}

void test_apply_move_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B..k...."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,3};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {1,0}, (Square) {1,4}});
    TEST_ASSERT_EQUAL_STRING_CHESS("B..k...."
                                   ".r...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "....Q..."
                                   "........", board_string(new_state));
}

void test_apply_move_02_uneventful_moves_add(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B..k...."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,3};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {1,0}, (Square) {1,4}});
    TEST_ASSERT_TRUE(state.uneventful_moves + 1 == new_state->uneventful_moves);
}

void test_apply_move_03_capturing(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B..k...."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,3};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,0}, (Square) {6,1}});
    TEST_ASSERT_EQUAL_STRING_CHESS("...k...."
                                   ".B...P.."
                                   "........"
                                   "P...B..."
                                   "Kn......"
                                   ".p......"
                                   "Q......."
                                   "........", board_string(new_state));
}

void test_apply_move_04_uneventful_moves_reset(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B..k...."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,3};
    state.uneventful_moves = 10;
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,0}, (Square) {6,1}});
    TEST_ASSERT_TRUE(0 == new_state->uneventful_moves);
}

void test_apply_move_05_pawn_upgradable(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "B..k...."
                              ".r...P.."
                              "........"
                              "P...B..."
                              "Kn......"
                              ".p......"
                              "Q......."
                              "........");
    state.king_white = (Square) {3,0};
    state.king_black = (Square) {7,3};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {6,5}, (Square) {7,5}});
    TEST_ASSERT_TRUE(true == new_state->pawn_upgradable);
}

void test_apply_move_06_castling_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {0,4}, (Square) {0,6}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r...k..r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R....RK.", board_string(new_state));
}

void test_apply_move_07_castling_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {0,4}, (Square) {0,2}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r...k..r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "..KR...R", board_string(new_state));
}

void test_apply_move_08_castling_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;     // active_player == BLACK
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,4}, (Square) {7,6}});
    TEST_ASSERT_EQUAL_STRING_CHESS("r....rk."
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R...K..R", board_string(new_state));
}

void test_apply_move_09_castling_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;     // active_player == BLACK
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,4}, (Square) {7,2}});
    TEST_ASSERT_EQUAL_STRING_CHESS("..kr...r"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "........"
                                   "R...K..R", board_string(new_state));
}

void test_apply_move_10_castling_legality_01(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {0,4}, (Square) {1,4}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_white)
                  && (false == new_state->castle_qensde_legal_white));
}

void test_apply_move_11_castling_legality_02(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {0,7}, (Square) {7,7}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_white)
                  && (true == new_state->castle_qensde_legal_white));
}

void test_apply_move_12_castling_legality_03(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {0,0}, (Square) {0,1}});
    TEST_ASSERT_TRUE((true == new_state->castle_kngsde_legal_white)
                  && (false == new_state->castle_qensde_legal_white));
}

void test_apply_move_13_castling_legality_04(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;     // active_player == BLACK
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,4}, (Square) {7,5}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_black)
                  && (false == new_state->castle_qensde_legal_black));
}

void test_apply_move_14_castling_legality_05(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;     // active_player == BLACK
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,0}, (Square) {5,0}});
    TEST_ASSERT_TRUE((true == new_state->castle_kngsde_legal_black)
                  && (false == new_state->castle_qensde_legal_black));
}

void test_apply_move_15_castling_legality_06(void)
{
    Game_state state;
    set_test_game_state(&state);    // active_player == WHITE
    state.move_number = 2;     // active_player == BLACK
    board_from_string(&state, "r...k..r"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "........"
                              "R...K..R");
    state.king_white = (Square) {0,4};
    state.king_black = (Square) {7,4};
    Game_state *new_state = apply_move(&state, (Move) {(Square) {7,7}, (Square) {7,6}});
    TEST_ASSERT_TRUE((false == new_state->castle_kngsde_legal_black)
                  && (true == new_state->castle_qensde_legal_black));
}


int main(void)
{
    UNITY_BEGIN();

    // core_functions.h
    RUN_TEST(test_board);
    RUN_TEST(test_is_attacked_by_rook);
    RUN_TEST(test_is_attacked_by_bishop);
    RUN_TEST(test_is_attacked_by_knight);
    RUN_TEST(test_is_attacked_by_pawn);
    RUN_TEST(test_is_king_square_white);
    RUN_TEST(test_is_king_square_black);
    RUN_TEST(test_active_player_white);
    RUN_TEST(test_active_player_black);
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
    RUN_TEST(test_apply_move_01);
    RUN_TEST(test_apply_move_02_uneventful_moves_add);
    RUN_TEST(test_apply_move_03_capturing);
    RUN_TEST(test_apply_move_04_uneventful_moves_reset);
    RUN_TEST(test_apply_move_05_pawn_upgradable);
    RUN_TEST(test_apply_move_06_castling_01);
    RUN_TEST(test_apply_move_07_castling_02);
    RUN_TEST(test_apply_move_08_castling_03);
    RUN_TEST(test_apply_move_09_castling_04);
    RUN_TEST(test_apply_move_10_castling_legality_01);
    RUN_TEST(test_apply_move_11_castling_legality_02);
    RUN_TEST(test_apply_move_12_castling_legality_03);
    RUN_TEST(test_apply_move_13_castling_legality_04);
    RUN_TEST(test_apply_move_14_castling_legality_05);
    RUN_TEST(test_apply_move_15_castling_legality_06);

    return UNITY_END();
}

