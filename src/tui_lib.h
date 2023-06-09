// Copyright: (c) 2023, Alrik Neumann
// GNU General Public License v3.0+ (see LICENSE.txt or https://www.gnu.org/licenses/gpl-3.0.txt)

/********************************************************************
 * tui_lib.h
 * Library for a simple tui.
 *
 * offers constructor and destructor for screen-objects
 * A screen is basically a linked list with pointers to windows as
 * nodes.
 * When the screen is drawn the linked list will be traversed
 * drawing windows one on the other.
 * screen also has height and width.
 *
 * offers constructor and destructor for window-objects
 *
 * window objects have height, width, position and can be
 * fed with strings.
 *
 * position denotes the upper-left corner of the window-object
 * relative to the defined screen.
 *
 * priority determines which window will be drawn on top of other
 * windows and which window will be drawn below.
 * A challange will be to keep track of window priorities,
 * as no two windows should have the same priority.
 *
 ********************************************************************/
#ifndef TUI_LIB_H
#define TUI_LIB_H

#include <stdbool.h>

typedef struct screen *Screen;
typedef struct window *Window;

typedef enum orientation {
    LEFT = 1,
    CENTER = 2,
    RIGHT = 3,
} Orientation;

typedef enum linebreak {
    LB_NORMAL = 1,
    LB_TRUNCATE = 2,
    LB_SMART = 3,
} Linebreak;

/********************************************************************
 * window_create: every window is assigned a unique identifier
 *                when created. This is the way in which
 *                screen_remove_window() can find the Window passed
 *                as argument.
 *                Maximum window number will be set by
 *                preprocessor-directive.
 ********************************************************************/
Window window_create(void);

/********************************************************************
 * screen_create: Creates a new screen without any windows.
 ********************************************************************/
Screen screen_create(void);

/********************************************************************
 * screen_set_size: Sets height and width of screen.
 *                  Returns false if either either of the to size
 *                  arguments is negative.
 ********************************************************************/
bool screen_set_size(Screen screen, int height, int width);

/********************************************************************
 * screen_add_window: Adds a pointer to window to screen with
 *                    priority.
 *                    Returns true on succsess.
 *                    When priority is already used in screen,
 *                    does not add window and returns false.
 ********************************************************************/
bool screen_add_window(Screen screen, Window window, int priority);

/********************************************************************
 * screen_remove_window: Removes window from screen i.e. removes
 *                       a the first node with a pointer to window
 *                       from the linked list which is screen.
 *                       screen.
 *                       true on success
 *                       false if not found
 *                       Finds window by it's unique identifier.
 ********************************************************************/
bool screen_remove_window(Screen screen, Window window);

/********************************************************************
 * window_destroy: Before a window is destroyed, it should be
 *                 removed from all screens. Otherwise attempting
 *                 to draw a screen that containing a node pointing
 *                 to the window will crash the program.
 ********************************************************************/
void window_destroy(Window window);

/********************************************************************
 * screen_destroy: Does NOT destroy the associated windows!
 ********************************************************************/
void screen_destroy(Screen screen);

//////////////////// below this line everything is untested //////////////////

/********************************************************************
 * screen_window_set_position: Sets the position of a window inside
 *                             of a screen.
 *                             Returns false if window is not in
 *                             screen.
 ********************************************************************/
bool screen_window_set_position(Screen screen, Window window, int pos_hori, int pos_vert);

/********************************************************************
 * window_set_frame: Defines how the frame of a window is to be
 *                   displayed.
 *                   Needs to be used in conjunction with
 *                   window_display_frame() as by itself it will only
 *                   control HOW a frame would be displayed, but not
 *                   IF it is displayed at all.
 *                   Only accepts printable characters as input.
 *                   default for all three values is ' '
 *
 ********************************************************************/
bool window_set_frame(Window window, char delim_hori, char delim_vert, char delim_corner);

/********************************************************************
 * window_set_orientation: Defines the orientation with which the
 *                         content of a window will be displayed.
 *                         accepted arguments for orientation:
 *                         LEFT, CENTER, RIGHT
 ********************************************************************/
bool window_set_orientation(Window window, Orientation orientation);

/********************************************************************
 * window_set_space: Defines how much space should be left between
 *                   the windows content and it's boarder or it's
 *                   frame if the latter is set to be displayed.
 ********************************************************************/
bool window_set_space(Window window, int top, int bot, int left, int right);

/********************************************************************
 * window_display_frame: Defines if the window is displayed with a
 *                       frame.
 *                       default = false
 ********************************************************************/
void window_display_frame(Window window, bool display);

/********************************************************************
 * window_set_size: Sets the sets the size of a window.
 *                  Returns false if one of the give values is
 *                  negative.
 ********************************************************************/
bool window_set_size(Window window, int height, int width);

/********************************************************************
 * window_set_linebreak: Specifies how linebreaks are handled inside
 *                       of the window.
 *                       LB_NORMAL = content-line continues at the
 *                       next display-line
 *                       LB_TRUNCATE = content-line gets truncated
 *                       at the end of display-line
 *                       LB_SMART = linebreak happens after the
 *                       last word in a content-line, that fits onto
 *                       a display-line
 ********************************************************************/
bool window_set_linebreak(Window window, Linebreak lb_mode);

/********************************************************************
 * window_update_content: Rewrites the content (i.e. the string
 *                        which will be displayed when the window
 *                        is drawn as part of a screen) of a window.
 *                        Returns false, if content_length is
 *                        negative.
 ********************************************************************/
bool window_update_content(Window window, char *content, int content_length);

/********************************************************************
 * window_set_fill: Sets a windows filler characters.
 *                  fill_border = used to fill the space between the
 *                  border of a window and the content.
 *                  fill_line = used to fill the remaining positions
 *                  in the content area when a line or the content
 *                  ended.
 ********************************************************************/
bool window_set_fill(Window window, char fill_line, char fill_border);

/********************************************************************
 * screen_duplicate: Creates a duplicate screen.
 *                   Does NOT duplicate the windows.
 *                   The duplicate-screen's nodes will point to the
 *                   same windows, as the original-screen's nodes do.
 ********************************************************************/
//Screen screen_duplicate(Screen screen);

/********************************************************************
 * window_duplicate: Returns a duplicate of a window.
 *                   The duplicate is an exact copy of the original.
 *                   The only difference are their unique ids and
 *                   the fact, that the duplicate won't be part of
 *                   any screens when it is created, even if the
 *                   original already is part of one or multiple
 *                   screens.
 ********************************************************************/
Window window_duplicate(Window window);

////////////////////////////
//  Functions for output  //
////////////////////////////
/********************************************************************
 * screen_print: Prints all the windows in screen one over the other
 *               according to their priorities. Highest priority on
 *               top.
 *               This happens by creating and repeatedly modifying
 *               a string, which will be printed in the end.
 ********************************************************************/
void screen_print(Screen screen);

/********************************************************************
 * window_print_content: returns the id of the window.
 ********************************************************************/
void window_print_content(Window window);

/********************************************************************
 * window_print: Prints the content of a window wiht all specified
 *               parameters.
 *               Not yet implemented:
 *               display_modes
 *               orientation
 ********************************************************************/
void window_print(Window window);
#endif
