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
 * screen_add_content: Fills string into window height and width
 *                     formatting according to heigth and length
 *                     is ignored until window gets accessed as
 *                     part of a call of draw_screen().
 *                     Returns false if content_length < 0;
 *                     If content is longer than content_length it
 *                     gets truncated.
 ********************************************************************/
bool window_update_content(Window window, char *content, int content_length);

/********************************************************************
 ********************************************************************/
Screen screen_create(void);

/********************************************************************
 * screen_duplicate: Creates a duplicate screen.
 *                   Does NOT duplicate the windows.
 *                   The duplicate-screen's nodes will point to the
 *                   same windows, as the original-screen's nodes do.
 ********************************************************************/
Screen screen_duplicate(Screen screen);

/********************************************************************
 * screen_destroy: Does NOT destroy the associated windows!
 ********************************************************************/
void screen_destroy(Screen screen);

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
 ********************************************************************/
Window window_duplicate(Window window);

/********************************************************************
 * window_destroy: Before a window is destroyed, it should be
 *                 removed from all screens. Otherwise attempting
 *                 to draw a screen that containing a node pointing
 *                 to the window will crash the program.
 ********************************************************************/
void window_destroy(Window window);

bool window_set_size(Window window, int height, int width);

bool window_set_position(Window window, int height, int width);

/********************************************************************
 * destroy_all_windows: Destroys all windows.
 *                      This is achieved by adding each created Screen
 *                      and Window to a globally accessible linked list.
 *                      which will be completely freed, when destroy_all()
 *                      is called.
 ********************************************************************/
void destroy_all_windows(void);
