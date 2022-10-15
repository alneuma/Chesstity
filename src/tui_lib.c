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
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

// Denotes the maximum number of windows/screens, that can exist at the same time.
// Keeps track of the amount of created windows/screens. Assigns a unique id to each window/screen.
#define MAX_WINDOWS_SCREENS_ID INT_MAX

typedef enum {
    LEFT_i = 1;
    CENTER_i = 2;
    RIGHT_i = 3;
} Orientation_i;

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
    // space at boarder (all >= 0)
    int space_top;
    int space_bot;
    int space_left;
    int space_right;
    // content variables
    Orientation_i content_orientation;
    int content_length;
    char *content;
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
    new_window->content_orientation = LEFT_i;
    new_window->content_length = 0;
    // +1 for '\0'
    new_window->content = malloc((new_window->content_length + 1) * sizeof(*new_window->content));
    MEM_TEST(new_window->content);

    *(new_window->content + 1) = '\0';

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
    new_screen->lowest = NULL;

    return new_screen;
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
    // old_position will end up pinting to the node.
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

    free(screen);
}

//
//
//
// functions below this line are not ready for testing yet
//
//
//
bool screen_draw(Screen screen, int height, int width);

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
bool window_set_frame(Window window, char delim_hori, char delim_vert, char corner)
{
    if (!isprint(delim_hori) || !isprint(delim_vert) || !isprint(corner))
        return false;
    window->delim_hori = delim_hori;
    window->delim_vert = delim_vert;
    window->delim_corner = delim_corner;

    return true;
}

/********************************************************************
 * window_set_space: Defines how much space should be left between
 *                   the windows content and it's boarder or it's
 *                   frame if the latter is set to be displayed.
 ********************************************************************/
bool window_set_space(Window window, int top, int, bot, int left, int right)
{
    if ((top < 0) || (bot < 0) || (left < 0) || (right < 0))
        return false;

    window->top = top;
    window->bot = bot;
    window->left = left;
    window->rigth = right;
    
    return true;
}

/********************************************************************
 * window_display_frame: Defines if the window is displayed with a
 *                       frame.
 *                       default = false
 ********************************************************************/
void window_display_frame(Window window, bool display)
{
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

    window->height = height;
    window->width = width;

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
    }
    return false;
}

/********************************************************************
 * window_set_orientation: Defines the orientation with which the
 *                         content of a window will be displayed.
 *                         accepted arguments for orientation:
 *                         LEFT, CENTER, RIGHT
 ********************************************************************/
bool window_set_orientation(Window window, Orientation orientation)
{
    if ((LEFT_i != orientation) && (CENTER_i != orientation) && (RIGHT_i != orientation))
        return false;

    window->content_orientation = orientation;

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
    duplicate_window = window_create(); // gives the duplicate a different id which is desired

    duplicate_window->height = window->height;
    duplicate_window->width = window->width;
    // frame variables (default false and ' ')
    duplicate_window->display_frame = window->display_frame;
    duplicate_window->delim_hori = window->delim_hori;
    duplicate_window->delim_vert = window->delim_vert;
    duplicate_window->delim_corner = window->delim_corner;
    // space at boarder (all >= 0)
    duplicate_window->space_top = window->space_top;
    duplicate_window->space_bot = window->space_bot;
    duplicate_window->space_left = window->space_left;
    duplicate_window->space_right = window->space_right;
    // content variables
    duplicate_window->content_orientation = window->content_orientation;
    duplicate_window->content_length = window->content_length;
    // +1 for '\0'
    duplicate_window->content = malloc((duplicate_window->content_length + 1) * sizeof(duplicate_window->content));
    MEM_TEST(duplicate_window->content);

    strcpy(duplicate_window->content, window->content);

    return duplicate_window;
}

Screen screen_duplicate(Screen screen)
{
    //Screen duplicate_screen = screen_create(void);
}
