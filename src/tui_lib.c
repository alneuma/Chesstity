// Copyright: (c) 2023, Alrik Neumann
// GNU General Public License v3.0+ (see LICENSE.txt or https://www.gnu.org/licenses/gpl-3.0.txt)

/********************************************************************
 * tui_lib.c
 *
 * This file implements two objects Screen and Window
 * Use tui_lib.h to access the 
 *
 * Overview Screen:
 * Screens are linked lists of nodes pointing to windows.
 * Each node in a Screen has a priority assigned to it.
 * The nodes in a Screen are ordered by priority.
 *
 * Overview Window:
 * Windows contain the following information:
 * height and width
 * position (denoted in height_pos and width_pos)
 * content (a string)
 * a list of all the screens it is in
 *
 * A screen can be drawn, with specified height and width.
 * This effectively creates a string of ' ' and '\n' wich repeatedly
 * gets partly overriden with the content of the screen's windows.
 * In the end this string will be sent to stdout with printf().
 *
 ********************************************************************/
// comment out the following line to disable debugging
#define DEBUG
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

#include "ds_lib.h"
#include "tui_lib.h"
#include "mem_utilities.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

// Denotes the maximum number of windows/screens, that can exist at the same time.
// Keeps track of the amount of created windows/screens. Assigns a unique id to each window/screen.
#define MAX_WINDOWS_SCREENS_ID INT_MAX

#define MIN(x, y) (((x) <= (y)) ? (x) : (y))
#define MAX(x, y) (((x) >= (y)) ? (x) : (y))

typedef enum orientation_i {
    LEFT_i = 1,
    CENTER_i = 2,
    RIGHT_i = 3,
} Orientation_i;

typedef enum linebreak_i {
    LB_NORMAL_i = 1,
    LB_TRUNCATE_i = 2,
    LB_SMART_i = 3,
} Linebreak_i;

// nodes of Window_screen_list, see below
typedef struct node_ptr_to_screen {
    Screen screen;
    struct node_ptr_to_screen *next;
} Node_ptr_to_screen;

// This is used inside of Window to safe all the screens a window is part of
// It is necessary to save them, so that when a window gets destroyed
// all the screens containing it can be accessed to remove the window from them.
typedef struct window_screen_list {
    Node_ptr_to_screen *first;
} *Window_screen_list;

// typedef in tui_lib.h
struct window {
    int id;
    // size (>= 0)
    int height;
    int width;
    // frame variables (default false and ' ')
    bool display_frame;
    char delim_hori;
    char delim_vert;
    char delim_corner;
    // filler variables
    char fill_line;
    char fill_border;
    // space at border (all >= 0)
    int space_top;
    int space_bot;
    int space_left;
    int space_right;
    // content variables
    Orientation_i content_orientation;
    Linebreak_i content_lb_mode;
    int content_length;
    char *content;
    // display_strings;
    char **display_strings;
    bool changed;
    int old_height;
    Window_screen_list screens;
};

// Nodes for screens
typedef struct node_ptr_to_window {
    int priority;
    int pos_hori;
    int pos_vert;
    Window window;
    struct node_ptr_to_window *next;
} Node_ptr_to_window;

// typedef in tui_lib.h
struct screen {
    int id;
    int height;
    int width;
    char background;
    bool changed;
    int old_height;
    char **display_strings;
    Node_ptr_to_window *lowest;
};

// keeps track of available ids for windows/screens
// ids of destroyed windows/screens go onto the stack
// when creating a new window/screen it is first looked for
// available ids on the stack.
// If none is present, the counter will be increased by one.
PRIVATE int id_counter = 1;
PRIVATE Stack_int returned_ids = NULL;

PRIVATE Window_screen_list screen_list_create(void);
PRIVATE void window_add_screen(Window window, Screen screen);
PRIVATE void window_remove_screen(Window window, Screen screen);
PRIVATE void window_screen_list_destroy(Window window);
PRIVATE void screen_remove_window_simple(Screen screen, Window window);
PRIVATE int get_id(void);
PRIVATE char **window_make_strings(Window window);
PRIVATE void window_update_strings(Window window);
PRIVATE char **screen_make_strings(Screen screen);
PRIVATE void screen_update_strings(Screen screen);

/********************************************************************
 * get_id: Returns a valid unique id.
 *         0 is reserved as errorcode.
 *         When 0 is returned no more valid unique ids are available.
 ********************************************************************/
PRIVATE int get_id(void)
{
    if (NULL == returned_ids)
    {
        returned_ids = stack_int_create();
        if (id_counter > MAX_WINDOWS_SCREENS_ID)
            return 0;
        return id_counter++;
    }

    int id;
    if (!stack_int_pop(returned_ids, &id))
    {
        if (id_counter > MAX_WINDOWS_SCREENS_ID)
            return 0;
        else
        {
            if (0 == id_counter)
                id_counter++;
            id = id_counter++;
        }
    }
    return id;
} 

/********************************************************************
 * window_create: Returns NULL if no more ids are available.
 *                During normal use this should rarely happen as
 *                there is a total of INT_WIDTH ids available.
 *
 *                Default values are 0 for everything with content
 *                of length 1 and '\0' as it's only character.
 ********************************************************************/
Window window_create(void)
{
    // checking for free id
    int id;
    if (!(id = get_id()))
    {
        printf("error: %s: maximum number of windows exceeded; returning NULL instead of new window\n", __func__);
        return NULL;
    }

    // creating new window
    Window new_window = malloc(sizeof(*new_window));
    MEM_TEST(new_window);

    // setting default values
    new_window->id = id;
    new_window->height = 0;
    new_window->width = 0;
    new_window->space_top = 0;
    new_window->space_bot = 0;
    new_window->space_left = 0;
    new_window->space_right = 0;
    new_window->display_frame = false;
    new_window->delim_hori = ' ';
    new_window->delim_vert = ' ';
    new_window->delim_corner = ' ';
    new_window->fill_line = ' ';
    new_window->fill_border = ' ';
    new_window->content_orientation = LEFT_i;
    Linebreak_i content_lb_mode = LB_NORMAL_i;
    new_window->content_length = 0;
    // +1 for '\0'
    new_window->content = malloc((new_window->content_length) * sizeof(*new_window->content));
    MEM_TEST(new_window->content);
    *(new_window->content + new_window->content_length) = '\0';

    new_window->display_strings = window_make_strings(new_window);
    new_window->changed = false;
    new_window->old_height = 0;

    new_window->screens = screen_list_create();

    return new_window;
}

/********************************************************************
 * screen_list_create: Only used inside of window_create() to create
 *                     the list to save screens of which the window
 *                     is part.
 ********************************************************************/
PRIVATE Window_screen_list screen_list_create(void)
{
    Window_screen_list new_screen_list = malloc(sizeof(*new_screen_list));
    MEM_TEST(new_screen_list);

    new_screen_list->first = NULL;
    return new_screen_list;
}

/********************************************************************
 * screen_remove_window_simple: Removes a window from a screen
 *                              without error checking and without
 *                              removing the screen from the
 *                              windows's screen_list.
 *
 *                              Only used in
 *                              window_screen_list_destroy().
 *
 *                              In future could be used inside of
 *                              a function, that removes a window
 *                              from all screens.
 ********************************************************************/
PRIVATE void screen_remove_window_simple(Screen screen, Window window)
{
    Node_ptr_to_window *p = screen->lowest;
    Node_ptr_to_window *p_prev = NULL;
    while (p->window->id != window->id)
    {
        p_prev = p;
        p = p->next;
    }

    if (NULL != p_prev)
        p_prev->next = p->next;
    else
        screen->lowest = p->next;

    free(p);

    screen->changed = true;
}

/********************************************************************
 * window_screen_list_destroy: So far only used in window_destroy().
 *                             Uses screen_remove_window_simple() to
 *                             make sure, that all references to
 *                             window disappear from all screens.
 ********************************************************************/
PRIVATE void window_screen_list_destroy(Window window)
{
    Node_ptr_to_screen *p = window->screens->first;
    Node_ptr_to_screen *temp = NULL;
    while (NULL != p)
    {
        screen_remove_window_simple(p->screen, window);
        temp = p;
        p = p->next;
        free(temp);
    }
    free(window->screens);
}

/********************************************************************
 * window_add_screen: No error checking:
 *                    Assumes, that screen is not already in the
 *                    window's list of screens.
 *                    So far only used in screen_add_window().
 ********************************************************************/
PRIVATE void window_add_screen(Window window, Screen screen)
{
    Node_ptr_to_screen *new_node = malloc(sizeof(*new_node));
    MEM_TEST(new_node);

    new_node->screen = screen;
    new_node->next = window->screens->first;
    window->screens->first = new_node;
}

/********************************************************************
 * window_remove_screen: No error checking:
 *                       Assumes, that screen is in the window's
 *                       list of screens.
 *                       So far only used in screen_remove_window().
 *                       Does NOT remove the window from the screen
 *                       by itself, only removes the screen from the
 *                       window!
 ********************************************************************/
PRIVATE void window_remove_screen(Window window, Screen screen)
{
    Node_ptr_to_screen *p = window->screens->first;
    Node_ptr_to_screen *p_prev = NULL;
    while (p->screen->id != screen->id)
    {
        p_prev = p;
        p = p->next;
    }

    if (NULL == p_prev)
        window->screens->first = p->next;
    else
        p_prev->next = p->next;

    free(p);
}

/********************************************************************
 * screen_create: Creates a screen with no windows.
 ********************************************************************/
Screen screen_create(void)
{
    // checking for free id
    int id;
    if (!(id = get_id()))
    {
        printf("error: %s: maximum number of screens exceeded; returning NULL instead of new screen\n", __func__);
        return NULL;
    }

    Screen new_screen = malloc(sizeof(*new_screen));
    MEM_TEST(new_screen);

    new_screen->id = id;
    new_screen->height = 0;
    new_screen->width = 0;
    new_screen->lowest = NULL;
    new_screen->background = ' ';

    new_screen->changed = false;
    new_screen->old_height = 0;
    new_screen->display_strings = NULL;

    return new_screen;
}

bool screen_set_background(Screen screen, char background)
{
    if (!isprint(background))
        return false;

    screen->background = background;
    screen->changed = true;
}

/********************************************************************
 * screen_set_size: Sets height and width of screen.
 *                  Returns false if either either of the to size
 *                  arguments is negative.
 ********************************************************************/
bool screen_set_size(Screen screen, int height, int width)
{
    if ((height < 0) || (width < 0))
        return false;

    screen->old_height = screen->height;
    screen->height = height;
    screen->width = width;
    screen->changed = true;

    return true;
}

/********************************************************************
 * screen_add_window: Adds a node with given priority to screen,
 *                    pointing to window.
 *                    If window already is in screen, change the
 *                    priority of its node to the given one.
 *                    If priority is already occupied, downgrade all
 *                    existing priorities lesser or equal to it by
 *                    one, so that window can have the priority.
 *                    Returns false, if downgrade is requested,
 *                    but not possible due to integer overflow.
 ********************************************************************/
bool screen_add_window(Screen screen, Window window, int priority)
{
    // simplify the process if screen is empty
    // also test is necessary to make sure that screen->lowest
    // can be safely accessed in the following
    if (NULL == screen->lowest)
    {
        Node_ptr_to_window *new_node = malloc(sizeof(*new_node));
        MEM_TEST(new_node);
        new_node->window = window;
        new_node->priority = priority;
        new_node->pos_hori = 0;
        new_node->pos_vert = 0;
        new_node->next = NULL;
        screen->lowest = new_node;
        window_add_screen(window, screen);
        return true;
    }

    // check for possibility to downgrade existing priorities
    bool downgrade_prios_legal = true;
    if (INT_MIN >= screen->lowest->priority)
        downgrade_prios_legal = false;

    // new_position will point to the node,
    // where the node pointing to window will point to with ->next.
    // new_position_prev will point to the node,
    // that will point with ->next to the node pointing to window.
    // old_position will end up pointing to the node.
    // that points to window.
    // old_position_prev will point to the node,
    // before the node that is pointed to by old_position.
    Node_ptr_to_window *new_position = NULL;
    Node_ptr_to_window *new_position_prev = NULL;
    Node_ptr_to_window *old_position = NULL;
    Node_ptr_to_window *old_position_prev = NULL;

    // find the new_position, where window's node should be inserterd
    // if window already is in screen find it's current position i.e. old_position
    Node_ptr_to_window *p = screen->lowest;
    Node_ptr_to_window *p_prev = NULL;
    while ((NULL != p) && ((NULL == old_position) || (NULL == new_position)))
    {
        // find new position
        if ((NULL == new_position) && (p->priority >= priority))
        {
            if (p->priority == priority)
            {
                // check if the found node already points to window
                if (p->window->id == window->id)
                    return true;
                else if (downgrade_prios_legal)
                {
                    //downgrade priorities
                    Node_ptr_to_window *q = screen->lowest;
                    while ((NULL != q) && (q != p))
                    {
                        q->priority--;
                        q = q->next;
                    }
                    p->priority--;
                }
                else
                    return false;

                new_position = p->next;
                new_position_prev = p;
            }
            else
            {
                new_position = p;
                new_position_prev = p_prev;
            }
        }
        // find old position
        if ((NULL == old_position) && (p->window->id == window->id))
        {
            // if window already has priority, return
            if (p->priority == priority)
                return true;
            // otherwise save it's position in old_position
            else
            {
                old_position = p;
                old_position_prev = p_prev;
            }
        }
        // advance search
        p_prev = p;
        p = p->next;
    }

    // if priority is larger than all priorities existing in screen
    // prepare for inserting windows node at the end of the list
    if (NULL == new_position)
    {
        if (p_prev != old_position)
            new_position_prev = p_prev;
        else
        {
            old_position->priority = priority;
            return true;
        }
    }
    else if (new_position == old_position)
    {
        old_position->priority = priority;
        return true;
    }

    // create a new node for the window if it is not yet in screen
    // otherwise change the nodes position form old_position to new_position
    if (NULL == old_position)
    {
        Node_ptr_to_window *new_node = malloc(sizeof(*new_node));
        MEM_TEST(new_node);
        new_node->window = window;
        new_node->priority = priority;
        new_node->pos_hori = 0;
        new_node->pos_vert = 0;
        new_node->next = new_position;
        if (NULL == new_position_prev)
            screen->lowest = new_node;
        else
            new_position_prev->next = new_node;
        window_add_screen(window, screen);
    }
    else
    {
        if (NULL == old_position_prev)
            screen->lowest = old_position->next;
        else
            old_position_prev->next = old_position->next;
        if (NULL == new_position_prev)
            screen->lowest = old_position;
        else
            new_position_prev->next = old_position;
        old_position->next = new_position;
        old_position->priority = priority;
    }

    screen->changed = true;
    return true;
}

/********************************************************************
 * screen_remove_window: Removes the given window from the given
 *                       screen.
 *                       Uses window_remove_screen, to make sure,
 *                       that the screen will also disappeat from
 *                       the windows list of screens.
 ********************************************************************/
bool screen_remove_window(Screen screen, Window window)
{
    Node_ptr_to_window *p = screen->lowest;
    Node_ptr_to_window *p_prev = NULL;
    while (NULL != p)
    {
        if (p->window->id == window->id)
        {
            if (NULL == p_prev)
                screen->lowest = p->next;
            else
                p_prev->next = p->next;
            // remove screen from window
            window_remove_screen(window, screen);
            free(p);
            screen->changed = true;
            return true;
        }
        p_prev = p;
        p = p->next;
    }

    return false;
}

/********************************************************************
 * window_destroy: Destroys a window.
 *                 Calls window_screen_list_destroy, to makes sure,
 *                 that all references to it disappear from all
 *                 screens.
 ********************************************************************/
void window_destroy(Window window)
{
    if (NULL == returned_ids)
        returned_ids = stack_int_create();

    stack_int_push(returned_ids, window->id);
    window_screen_list_destroy(window);
    free(window->content);

    for (char **p = window->display_strings;
            p < window->display_strings + ((window->changed) ? window->old_height : window->height);
            p++)
    {
        free(*p);
    }
    free(window->display_strings);

    free(window);
}

/********************************************************************
 * screen_destroy: Destroys a screen, removing it from all screen
 *                 lists of all windows by calling
 *                 window_remove_screen() on all of it's windows
 *                 before freeing memory.
 ********************************************************************/
void screen_destroy(Screen screen)
{
    if (NULL == returned_ids)
        returned_ids = stack_int_create();

    stack_int_push(returned_ids, screen->id);

    Node_ptr_to_window *p = screen->lowest;
    Node_ptr_to_window *temp = NULL;

    while (NULL != p)
    {
        window_remove_screen(p->window, screen);
        temp = p;
        p = p->next;
        free(temp);
    }

    if (NULL != screen->display_strings)
    {
        for (char **p = screen->display_strings;
                p < screen->display_strings + (screen->changed ? screen->old_height : screen->height);
                p++)
        {
            free(*p);
        }
        free(screen->display_strings);
    }
    free(screen);
}

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
bool window_set_frame(Window window, char delim_hori, char delim_vert, char delim_corner)
{
    if (!isprint(delim_hori) || !isprint(delim_vert) || !isprint(delim_corner))
        return false;
    window->delim_hori = delim_hori;
    window->delim_vert = delim_vert;
    window->delim_corner = delim_corner;

    window->changed = true;

    return true;
}

/********************************************************************
 * window_set_space: Defines how much space should be left between
 *                   the windows content and it's border or it's
 *                   frame if the latter is set to be displayed.
 ********************************************************************/
bool window_set_space(Window window, int top, int bot, int left, int right)
{
    if ((top < 0) || (bot < 0) || (left < 0) || (right < 0))
        return false;

    window->space_top = top;
    window->space_bot = bot;
    window->space_left = left;
    window->space_right = right;
    
    window->changed = true;

    return true;
}

/********************************************************************
 * window_display_frame: Defines if the window is displayed with a
 *                       frame.
 *                       default = false
 ********************************************************************/
void window_display_frame(Window window, bool display)
{
    window->changed = true;

    window->display_frame = display;
}

/********************************************************************
 * window_set_size: Sets the sets the size of a window.
 *                  Returns false if one of the give values is
 *                  negative.
 ********************************************************************/
bool window_set_size(Window window, int height, int width)
{
    if ((height < 0) || (width < 0))
        return false;
 
    window->old_height = window->height;
    window->height = height;
    window->width = width;

    window->changed = true;

    return true;
}

/********************************************************************
 * window_set_linebreak: Specifies how linebreaks are handled inside
 *                       of the window.
 *                       LB_NORMAL_i = content-line continues at the
 *                       next display-line
 *                       LB_TRUNCATE_i = content-line gets truncated
 *                       at the end of display-line
 *                       LB_SMART_i = linebreak happens after the
 *                       last word in a content-line, that fits onto
 *                       a display-line
 ********************************************************************/
bool window_set_linebreak(Window window, Linebreak lb_mode)
{
    if ((LB_NORMAL_i != lb_mode) && (LB_TRUNCATE_i != lb_mode) && (LB_SMART_i != lb_mode))
        return false;

    window->content_lb_mode = lb_mode;

    window->changed = true;

    return true;
}

/********************************************************************
 * screen_window_set_position: Sets the position of a window inside
 *                             of a screen.
 *                             Returns false if window is not in
 *                             screen.
 ********************************************************************/
bool screen_window_set_position(Screen screen, Window window, int pos_hori, int pos_vert)
{
    Node_ptr_to_window *p = screen->lowest;
    while (p != NULL)
    {
        if (p->window->id == window->id)
        {
            p->pos_hori = pos_hori;
            p->pos_vert = pos_vert;
            return true;
        }
        p = p->next;
    }
    return false;
}

/********************************************************************
 * window_set_orientation: Defines the orientation with which the
 *                         content of a window will be displayed.
 *                         accepted arguments for orientation:
 *                         1 = left, 2 = center, 3 = right
 ********************************************************************/
bool window_set_orientation(Window window, Orientation orientation)
{
    if ((LEFT_i != orientation) && (CENTER_i != orientation) && (RIGHT_i != orientation))
        return false;

    window->content_orientation = orientation;

    window->changed = true;

    return true;
}

/********************************************************************
 * window_set_fill: Sets a windows filler characters.
 *                  fill_border = used to fill the space between the
 *                  border of a window and the content.
 *                  fill_line = used to fill the remaining positions
 *                  in the content area when a line or the content
 *                  ended.
 ********************************************************************/
bool window_set_fill(Window window, char fill_line, char fill_border)
{
    if ((!isprint(fill_line)) || (!isprint(fill_border)))
        return false;

    window->fill_line = fill_line;
    window->fill_border = fill_border;

    window->changed = true;

    return true;
}

/********************************************************************
 * window_update_content: Rewrites the content (i.e. the string
 *                        which will be displayed when the window
 *                        is drawn as part of a screen) of a window.
 *                        Returns false, if content_length is
 *                        negative.
 ********************************************************************/
bool window_update_content(Window window, char *content, int content_length)
{
    // validate input
    if (content_length < 0)
        return false;

    // adjust window's content_length
    if (content_length != window->content_length)
    {
        window->content_length = content_length;
        window->content = realloc(window->content, (window->content_length + 1) * sizeof(window->content));
        MEM_TEST(window->content);
    }

    // copying
    for (int i = 0; i < content_length; i++)
        window->content[i] = content[i];
    window->content[content_length] = '\0';

    window->changed = true;

    return true;
}

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
{
    Window duplicate_window = window_create(); // gives the duplicate a different id which is desired

    duplicate_window->height = window->height;
    duplicate_window->width = window->width;
    // frame variables (default false and ' ')
    duplicate_window->display_frame = window->display_frame;
    duplicate_window->delim_hori = window->delim_hori;
    duplicate_window->delim_vert = window->delim_vert;
    duplicate_window->delim_corner = window->delim_corner;
    // fillers
    duplicate_window->fill_line = window->fill_line;
    duplicate_window->fill_border = window->fill_border;
    // space at border (all >= 0)
    duplicate_window->space_top = window->space_top;
    duplicate_window->space_bot = window->space_bot;
    duplicate_window->space_left = window->space_left;
    duplicate_window->space_right = window->space_right;
    // content variables
    duplicate_window->content_orientation = window->content_orientation;
    duplicate_window->content_lb_mode = window->content_lb_mode;
    duplicate_window->content_length = window->content_length;
    // +1 for '\0'
    duplicate_window->content = malloc((duplicate_window->content_length + 1) * sizeof(*duplicate_window->content));
    MEM_TEST(duplicate_window->content);

    strcpy(duplicate_window->content, window->content);

    duplicate_window->display_strings = window_make_strings(duplicate_window);
    duplicate_window->changed = false;
    duplicate_window->old_height = window->old_height;

    return duplicate_window;
}

/*
Screen screen_duplicate(Screen screen)
{
    //Screen duplicate_screen = screen_create(void);
}
*/

bool screen_draw(Screen screen);

/********************************************************************
 * window_print: Prints the content of a window with all specified
 *               parameters.
 ********************************************************************/
void window_print(Window window)
{
    window_update_strings(window);

    char **p = window->display_strings;
    for (p = window->display_strings; p < window->display_strings + window->height; p++)
    {
        puts(*p);
    }
}

/********************************************************************
 * screen_print: Updates a screen if necessary and then
 *               prints it.
 ********************************************************************/
void screen_print(Screen screen)
{
    screen_update_strings(screen);

    char **p = screen->display_strings;
    for (p = screen->display_strings; p < screen->display_strings + screen->height; p++)
    {
        puts(*p);
    }
}

/********************************************************************
 * window_update_strings: updates window->display_strings should
 *                        be called, before window gets printed
 *                        or written to a screen, if 
 *                        window->changed is true
 ********************************************************************/
PRIVATE void window_update_strings(Window window)
{
    if (window->changed)
    {
        for (char **p = window->display_strings; p < window->display_strings + window->old_height; p++)
        {
            free(*p);
        }
        free(window->display_strings);

        window->display_strings = window_make_strings(window);
        window->changed = false;
    }
}

PRIVATE void screen_update_strings(Screen screen)
{
    if (screen->changed)
    {
        if (NULL != screen->display_strings)
        {
            for (char **p = screen->display_strings; p < screen->display_strings + screen->old_height; p++)
            {
                free(*p);
            }
            free(screen->display_strings);
        }

        screen->display_strings = screen_make_strings(screen);
        screen->changed = false;
    }
}

PRIVATE char **screen_make_strings(Screen screen)
{
    char **screen_rows = malloc(screen->height * sizeof(*screen_rows));
    MEM_TEST(screen_rows);

    char **screen_current_row;
    char *screen_current_col;
    for (screen_current_row = screen_rows;
         screen_current_row < screen_rows + screen->height;
         screen_current_row++)
    {
        *screen_current_row = malloc((screen->width + 1) * sizeof(**screen_current_row));
        MEM_TEST(*screen_current_row);

        for (screen_current_col = *screen_current_row;
             screen_current_col < *screen_current_row + screen->width;
             screen_current_col++)
            *screen_current_col = screen->background;

        *screen_current_col = '\0';
    }

    int window_beg_row;         // the first row in windows strings which will be displayed on screen
    int screen_beg_row;         // the first row in screen strings on which will be written
    int screen_end_row;         // the last row in screen strings on which will be written
    int window_beg_col;         // the first column in windows strings which will be displayed on screen
    int screen_beg_col;         // the first column in screen strings on which will be written
    int screen_end_col;         // the last column in screen strings on which will be written

    char **window_current_row;
    char *window_current_col;
    
    Node_ptr_to_window *p = screen->lowest;
    while (NULL != p)
    {
        if ((p->pos_hori > (screen->height - 1))
         || (p->pos_vert > (screen->width - 1))
         || (p->pos_hori + p->window->height - 1 < 0)
         || (p->pos_vert + p->window->width - 1 < 0))
            continue;

        window_update_strings(p->window);

        window_beg_row = (p->pos_hori >= 0) ? 0 : -(p->pos_hori);
        window_beg_col = (p->pos_vert >= 0) ? 0 : -(p->pos_vert);
        screen_beg_row = (p->pos_hori >= 0) ? p->pos_hori : 0;
        screen_beg_col = (p->pos_vert >= 0) ? p->pos_vert : 0;
        screen_end_row = MIN(p->pos_hori + p->window->height - 1, screen->height - 1);
        screen_end_col = MIN(p->pos_vert + p->window->width - 1, screen->width - 1);

        // fill in window
        for (screen_current_row = screen_rows + screen_beg_row,
             window_current_row = p->window->display_strings + window_beg_row;
             screen_current_row <= screen_rows + screen_end_row;
             screen_current_row++, window_current_row++)
        {
            for (screen_current_col = *screen_current_row + screen_beg_col,
                 window_current_col = *window_current_row + window_beg_col;
                 screen_current_col <= *screen_current_row + screen_end_col;
                 screen_current_col++, window_current_col++)
            {
                *screen_current_col = *window_current_col;
            }
        }
        
        p = p->next;
    }

    return screen_rows;
}

PRIVATE char **window_make_strings(Window window)
{
    char **rows = malloc(window->height * sizeof(*rows));
    MEM_TEST(rows);

    char **current_row;
    for (current_row = rows; current_row < rows + window->height; current_row++)
    {
        *current_row = malloc((window->width + 1) * sizeof(**current_row));
        MEM_TEST(*current_row);
    }

    bool content_end = false;
    bool write = true;
    char *p = window->content;
    int i;

    int top_space = window->space_top + window->display_frame;
    int bot_space = window->space_bot + window->display_frame;
    int left_space = window->space_left + window->display_frame;
    int right_space = window->space_right + window->display_frame;

    for (current_row = rows; current_row < rows + window->height; current_row++)
    {
        write = true;
        for (i = 0; i < window->width; i++)
        {
            if ((current_row <= rows + top_space - 1)
             || (current_row >= rows + window->height - bot_space)
             || (i <= left_space - 1)
             || (i >= window->width - right_space))
            {
                if (window->display_frame)
                {
                    if ((current_row == rows) || (current_row == rows + window->height - 1))
                    {
                        if ((0 == i) || (window->width - 1 == i))
                        {
                            (*current_row)[i] = window->delim_corner;
                            continue;
                        }
                        else
                        {
                            (*current_row)[i] = window->delim_hori;
                            continue;
                        }
                    }
                    else if ((0 == i) || (window->width - 1 == i))
                    {
                        (*current_row)[i] = window->delim_vert;
                        continue;
                    }
                }
                (*current_row)[i] = window->fill_border;
            }

            // there needs to be window->display_mode  normal | truncate | smart 
            // implement it with typedef enum
            // the following implements normal
            
            else if (write && !content_end)
            {
                if (*p == '\n')
                {
                    (*current_row)[i] = window->fill_line;
                    write = false;
                    p++;
                }
                else if (*p == '\0')
                {
                    (*current_row)[i] = window->fill_line;
                    content_end = true;
                }
                else
                    (*current_row)[i] = *p++;
            }
            else
                (*current_row)[i] = window->fill_line;

            // this truncates is used for window->display_mode == truncate
            if ((LB_TRUNCATE_i == window->content_lb_mode)
                 && write && !content_end
                          && (current_row >= rows + top_space)
                          && (i == window->width - right_space))
            {
                while ((*p != '\n') && (*p != '\0'))
                    p++;
                if ('\0' == *p)
                    content_end = true;
                else
                    p++;
            }
        }
        (*current_row)[i] = '\0';
        
        // this part follows gets additionally executed, if instead of
        // window->display_mode == normal, it is
        // window->display_mode == truncate
        // for now it just does not get executed
        // missing is the implementation of window->display_mode == smart
    }
    
    return rows;
}
