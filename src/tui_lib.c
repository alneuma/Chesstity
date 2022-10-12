/********************************************************************
 * tui_lib.c
 *
 * This file implements two objects Screen and Window
 *
 * Overview Screen:
 * Screens are linked lists of nodes pointing to windows.
 * Each node in a Screen has a priority assigned to it.
 * The nodes in a Screen are ordered by priority.
 *
 * Overview Window:
 * Windows contain the following information:
 * heigth and width
 * position (denoted in height_pos and width_pos)
 * content (a string)
 * a list of all the screens it is in
 *
 * 
 ********************************************************************/
// settings to enable debugging
#define DEBUG
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

#include "ds_lib.h"
#include "tui_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

// Denotes the maximum number of windows/screens, that can exist at the same time.
// Keeps track of the amount of created windows/screens. Assigns a unique id to each window/screen.
#define MAX_WINDOWS_SCREENS INT_MAX

typedef struct node_ptr_to_screen {
    Screen *screen;
    struct node_ptr_to_screen *next;
} Node_ptr_to_screen;

typedef struct window_screen_list {
    Node_ptr_to_screen *first;
} *Window_screen_list;

struct window {
    int id;
    int pos_height;
    int pos_width;
    int height;
    int width;
    int content_length;
    char *content;
    Window_screen_list sreens;
};

struct screen {
    int id;
    Node_ptr_to_window *lowest;
};

typedef struct node_ptr_to_window {
    int priority;
    Window window;
    struct node_ptr_to_window *next;
} Node_ptr_to_window;

// keeps track of available ids for windows/screens
// ids that of destroyed windows/screens go onto the stack
// when creating a new window/screen first looks for
// available ids on the stack
PRIVATE int id_counter = 0;
PRIVATE struct stack_int {
    Stack_int_node *top = NULL;
} returned_ids;

PRIVATE Window_screen_list screen_list_create(void);
PRIVATE void window_add_screen(Window window, Screen screen);
PRIVATE void window_remove_screen(Window window, Screen screen);
PRIVATE void screen_list_destory(Window_screen_list screen_list);
PRIVATE void screen_remove_window_simple(Screen_screen, Window window);

/********************************************************************
 * MEM_TEST:
 ********************************************************************/
#define MEM_TEST(ptr) \
{ \
    if (NULL == (ptr)) \
    { \
        printf("error: %s: memory-allocation failed; aborting\n", __func__); \
        exit(EXIT_FAILURE); \
    } \
}

Screen screen_duplicate(Screen screen)
{
    //Screen duplicate_screen = screen_create(void);
}

Window window_create(void)
{
    // checking if windows can still be created
    // and setting id if it can
    int id;
    if (!stack_int_pop(returned_ids, &id))
    {
        if (id_counter > MAX_WINDOWS)
        {
            printf("error: %s: maximum number of windows exceeded; returning NULL instead of new window\n", __func__);
            return NULL;
        }
        else
        {
            id = id_counter++;
        }
    }

    // creating new window
    Window new_window = malloc(sizeof(new_window));
    MEM_TEST(new_window);

    // setting default values
    new_window->id = id;
    new_window->pos_height = 0;
    new_window->pos_width = 0;
    new_window->height = 0;
    new_window->width = 0;
    new_window->content_length = 0;
    // +1 for '\0'
    new_window->content = malloc((new_window->content_length + 1) * sizeof(new_window->content));
    MEM_TEST(new_window->content);

    *new_window->content = '\0';

    new_window->screens = screen_list_create();

    return new_window;
}

PRIVATE Window_screen_list screen_list_create(void)
{
    Window_screen_list *new_screen_list = malloc(sizeof(*new_screen_list));
    MEM_TEST(new_screen_list);

    return new_sreen_list;
}

PRIVATE void window_screen_list_destory(Window window)
{
    Node_ptr_to_screen *p = window->screen_list->first;
    Node_ptr_to_screen *temp = NULL;
    while (NULL != p)
    {
        screen_remove_window_simple(p->screen, window);
        temp = p;
        free(temp);
        p = p->next;
    }
    free(window->screen_list);
}

/********************************************************************
 * screen_remove_window_simple: Removes a window from a screen
 *                              without error checking and without
 *                              removing the screen from the
 *                              windows's screen_list.
 *                              Used for screen_list_destroy.
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
    p_prev->next = p->next;
    free(p);
}

// Assumes that screen is NOT already in window.
// This assumption can be made, because window_add_screen()
// will only be called by functions where this is already checked.
PRIVATE void window_add_screen(Window window, Screen screen)
{
    Node_ptr_to_screen *new_node = malloc(sizeof(*new_node));
    MEM_TEST(new_node);

    new_node->screen = screen;
    new_node->next = window->screens->first;
    window->screens->first = new_node;
}

// Assumes that screen IS in window.
// This assumption can be made, because window_add_screen()
// will only be called by functions where this is already checked.
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

    free(p)
}

Screen screen_create(void)
{
    // checking if screens can still be created
    // and setting id if it can
    int id;
    if (!stack_int_pop(returned_ids, &id))
    {
        if (id_counter > MAX_SCREENS)
        {
            printf("error: %s: maximum number of screens exceeded; returning NULL instead of new screen\n", __func__);
            return NULL;
        }
        else
        {
            id = id_counter++;
        }
    }

    Screen new_screen = malloc(sizeof(new_screen));
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
        screen->lowest = new_node;
        window_add_screen(window, screen);
    }

    // check for possibility to downgrade existing priorities
    bool downgrade_prios_legal = true;
    if (INT_MIN => screen->lowest->priority)
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
    Node_ptr_to_window *p;
    Node_ptr_to_window *p_prev;
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
                    while ((NULL != q) && (q->id != window->id))
                    {
                        q->priority--;
                        q = q->next;
                    }
                    p->priority--;
                }
                else
                    return false;
            }
            new_position = p;
            if (NULL != p_prev)
                new_position_prev = p_prev;
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
                if (NULL != p_prev)
                    old_position_prev = p_prev;
            }
        }
        // advance search
        p_prev = p;
        p = p->next;
    }

    // create a new node for the window if it is not yet in screen
    // otherwise change the nodes position form old_position to new_position
    if (NULL == old_position)
    {
        Node_ptr_to_window *new_node = malloc(sizeof(*new_node));
        MEM_TEST(new_node);
        new_node->window = window;
        new_node->priority = priority;
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

void window_destroy(Window window)
{
    window_screen_list_destroy(window);
    free(window->content);
    free(window);
}

void screen_destroy(Screen screen)
{
    Node_ptr_to_window *p = screen->next;
    Node_ptr_to_window *temp;

    while (NULL != p)
    {
        window_remove_screen(p->window, screen);
        temp = p;
        p = p->next;
        free(temp);
    }

    free(screen);
}

bool screen_draw(Screen screen, int height, int width);
bool window_set_size(Window window, int height, int width);
{
    if ((height < 0) || (width < 0))
        return false;

    window->height = heigth;
    window->width = width;

    return true;
}

bool window_set_position(Window window, int height, int width)
{
    if ((height < 0) || (width < 0))
        return false;

    window->pos_height = heigth;
    window->pos_width = width;

    return true;
}

bool window_update_content(Window window, char *content, int content_length)
{
    // validate input
    if (content_length < 0)
        return false;

    // adjust window's content_length
    if (content_length != window->content_length)
    {
        free(window->content);
        window->content_length = content_length;
        window->content = malloc((window->content_length + 1) * sizeof(window->content));
        MEM_TEST(window);
    }

    // copying
    for (int i = 0; i < content_length; i++)
        window->content[i] = content[i];
    window->content[content_length] = '\0';

    return true;
}

/*
Window window_duplicate(Window window)
{
    duplicate_window = window_create(); // gives the duplicate a different id which is desired

    duplicate_window->pos_height = window->pos_height;
    duplicate_window->pos_width = window->pos_width;
    duplicate_window->height = window->height;
    duplicate_window->width = window->width;
    duplicate_window->content_length = window->content_length;
    // +1 for '\0'
    duplicate_window->content = malloc((duplicate_window->content_length + 1) * sizeof(duplicate_window->content));
    MEM_TEST(duplicate_window);

    strcpy(duplicate_window->content, window->content);

    return duplicate_window;
}
*/

