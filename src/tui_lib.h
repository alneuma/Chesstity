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
 * window objects have height, width, priority, position and can be
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

typedef struct screen *Screen;
typedef struct window *Window;
/********************************************************************
 ********************************************************************/

/********************************************************************
 * screen_add_window: Adds a pointer to window to screen with
 *                    priority.
 *                    Returns true on succsess.
 *                    When priority is already used in screen,
 *                    does not add window and returns false.
 *                    Assigns id 
 ********************************************************************/
bool screen_add_window(Screen screen, Window window, int priority);

/********************************************************************
 * screen_remove_window: Removes window from screen i.e. removes
 *                       the pointer to window from the linked list
 *                       screen.
 *                       true on success
 *                       false if not found
 ********************************************************************/
bool screen_remove_window(Screen screen, Window window);

/********************************************************************
 * draw_screen: Prints all the windows in screen one over the other
 *              according to their priorities. Highes priority on
 *              top.
 ********************************************************************/
void draw_screen(screen);
