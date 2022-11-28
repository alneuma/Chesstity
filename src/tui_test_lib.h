/********************************************************************
 * tui_test_lib.h
 * Gives access to functions, which facilitate testing of tui_lib.c
 * and tui_lib.h
 ********************************************************************/
#ifndef TUI_TEST_LIB_H
#define TUI_TEST_LIB_H

#include "tui_lib.h"
#include <stdbool.h>

/********************************************************************
 * create_test_window: Creates a test window with specified size
 *                     and content.
 ********************************************************************/
Screen create_test_screen(int height, int width, char background);

/********************************************************************
 * create_test_window: Creates a test window with specified size
 *                     and content.
 ********************************************************************/
Window create_test_window(int height, int width);

/********************************************************************
 * screen_print_info: Prints id and priority of all windows in
 *                    screen.
 ********************************************************************/
void screen_print_info(Screen screen);

/********************************************************************
 * window_print_info: Prints the ids of all the screens, which
 *                    contain window.
 ********************************************************************/
void window_print_info(Window window);

/********************************************************************
 * screen_id: returns the id of the screen.
 ********************************************************************/
int screen_id(Screen screen);

/********************************************************************
 * window_id: returns the id of the window.
 ********************************************************************/
int window_id(Window window);

/********************************************************************
 * screen_windows_id: Returns a pointer to an array that contains
 *                    the ids of all the windows in screen.
 *                    Writes the total amount of windows in screen
 *                    to *amount.
 ********************************************************************/
int *screen_windows_id(Screen screen, int *amount);

/********************************************************************
 * screen_windows_prito: Returns a pointer to an array that contains
 *                       the priorities of all the windows in screen.
 *                       Writes the total amount of windows in screen
 *                       to *amount.
 ********************************************************************/
int *screen_windows_prio(Screen screen, int *amount);

/********************************************************************
 * window_screens_id: Returns a pointer to an array that contains
 *                    the ids of all the screens containing window.
 *                    Writes the total amount of screens to *amount.
 ********************************************************************/
int *window_screens_id(Window window, int *amount);

/********************************************************************
 * compare_arrays_int: Checks if the first length elements of two
 *                     int-arrays are equal and in the same order.
 ********************************************************************/
bool compare_arrays_int(int *ar_1, int *ar_2, int length);

/********************************************************************
 * compare_sorted_arrays_int: Checks if the first length elements of
 *                            two int-arrays are equal
 *                            (in any order).
 ********************************************************************/
bool compare_sorted_arrays_int(int *ar_1, int *ar_2, int length);

#endif
