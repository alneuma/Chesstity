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
 *
 ********************************************************************/
#include <stdbool.h>

typedef struct screen *Screen;
typedef struct window *Window;

typedef enum {
    LEFT = 1;
    CENTER = 2;
    RIGHT = 3;
} Orientation;

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
bool window_set_frame(Window window, char delim_hori, char delim_vert, char corner);

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
bool window_set_space(Window window, int top, int, bot, int left, int right);

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
 * screen_window_set_position: Sets the position of a window inside
 *                             of a screen.
 *                             Returns false if window is not in
 *                             screen.
 ********************************************************************/
bool screen_window_set_position(Screen screen, Window window, int pos_hori, int pos_vert);

/********************************************************************
 * screen_draw: Prints all the windows in screen one over the other
 *              according to their priorities. Highest priority on
 *              top.
 *              This happens by creating and repeatedly modifying
 *              a string, which will be printed in the end.
 *              Returns false if height or width is a negative
 *              negative number.
 ********************************************************************/
bool screen_draw(Screen screen, int height, int width);

/********************************************************************
 * window_update_content: Rewrites the content (i.e. the string
 *                        which will be displayed when the window
 *                        is drawn as part of a screen) of a window.
 *                        Returns false, if content_length is
 *                        negative.
 ********************************************************************/
bool window_update_content(Window window, char *content, int content_length);

/********************************************************************
 * screen_duplicate: Creates a duplicate screen.
 *                   Does NOT duplicate the windows.
 *                   The duplicate-screen's nodes will point to the
 *                   same windows, as the original-screen's nodes do.
 ********************************************************************/
Screen screen_duplicate(Screen screen);

/********************************************************************
 * window_duplicate: Returns a duplicate of a window.
 *                   The duplicate is an exact copy of the original.
 *                   The only difference are their unique ids and
 *                   the fact, that the duplicate won't be part of
 *                   any screens when it is created, even if the
 *                   original already is part of one or multiple
 *                   screens.
 ********************************************************************/
Window window_duplicate(Window window)
