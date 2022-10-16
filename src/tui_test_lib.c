// comment out the following line to disable debugging
//#define DEBUG
#ifdef DEBUG
#define PRIVATE
#else
#define PRIVATE static
#endif

#include "tui_lib.h"
#include "tui_test_lib.h"
#include "mem_utilities.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// necessary declarations from tui_lib.c
typedef struct node_ptr_to_screen {
    Screen screen;
    struct node_ptr_to_screen *next;
} Node_ptr_to_screen;

typedef struct window_screen_list {
    Node_ptr_to_screen *first;
} *Window_screen_list;

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
    int content_orientation;
    int content_length;
    char *content;
    Window_screen_list screens;
};

typedef struct node_ptr_to_window {
    int priority;
    int pos_hori;
    int pos_vert;
    Window window;
    struct node_ptr_to_window *next;
} Node_ptr_to_window;

struct screen {
    int id;
    int height;
    int width;
    Node_ptr_to_window *lowest;
};

PRIVATE int compare_ints(const void *a, const void *b);

/********************************************************************
 * screen_print_info: Prints id and priority of all windows in
 *                    screen.
 ********************************************************************/
void screen_print_info(Screen screen)
{
    printf("windows in screen %d:\n", screen->id);
    Node_ptr_to_window *p = screen->lowest;
    while (NULL != p)
    {
        printf("id = %3d prio = %3d\n", p->window->id, p->priority);
        p = p->next;
    }
}

/********************************************************************
 * window_print_info: Prints the ids of all the screens, which
 *                    contain window.
 ********************************************************************/
void window_print_info(Window window)
{
    printf("screens containing window %d:\n", window->id);
    Node_ptr_to_screen *p = window->screens->first;
    while (NULL != p)
    {
        printf("id = %3d\n", p->screen->id);
        p = p->next;
    }
}

/********************************************************************
 * window_print_content: returns the id of the window.
 ********************************************************************/
void window_print_content(Window window)
{
    puts(window->content);
}

/********************************************************************
 * screen_id: returns the id of the screen.
 ********************************************************************/
int screen_id(Screen screen)
{
    return screen->id;
}

/********************************************************************
 * window_id: returns the id of the window.
 ********************************************************************/
int window_id(Window window)
{
    return window->id;
}

/********************************************************************
 * screen_windows_id: Returns a pointer to an array that contains
 *                    the ids of all the windows in screen.
 *                    Writes the total amount of windows in screen
 *                    to *amount.
 ********************************************************************/
int *screen_windows_id(Screen screen, int *amount)
{
    *amount = 0;
    Node_ptr_to_window *p = screen->lowest;
    while (NULL != p)
    {
        (*amount)++;
        p = p->next;
    }

    int *ids = malloc(*amount * sizeof(*ids));
    MEM_TEST(ids);

    int *q = ids;
    p = screen->lowest;
    while (NULL != p)
    {
        *q++ = p->window->id;
        p = p->next;
    }

    return ids;
}

/********************************************************************
 * screen_windows_prito: Returns a pointer to an array that contains
 *                       the priorities of all the windows in screen.
 *                       Writes the total amount of windows in screen
 *                       to *amount.
 ********************************************************************/
int *screen_windows_prio(Screen screen, int *amount)
{
    *amount = 0;
    Node_ptr_to_window *p = screen->lowest;
    while (NULL != p)
    {
        (*amount)++;
        p = p->next;
    }

    int *priorities = malloc(*amount * sizeof(*priorities));
    MEM_TEST(priorities);

    int *q = priorities;
    p = screen->lowest;
    while (NULL != p)
    {
        *q = p->priority;
        p = p->next;
        q++;
    }

    return priorities;
}

/********************************************************************
 * window_screens_id: Returns a pointer to an array that contains
 *                    the ids of all the screens containing window.
 *                    Writes the total amount of screens to *amount.
 ********************************************************************/
int *window_screens_id(Window window, int *amount)
{
    *amount = 0;
    Node_ptr_to_screen *p = window->screens->first;
    while (NULL != p)
    {
        (*amount)++;
        p = p->next;
    }

    int *ids = malloc(*amount * sizeof(*ids));
    MEM_TEST(ids);

    int *q = ids;
    p = window->screens->first;
    while (NULL != p)
    {
        *q = p->screen->id;
        p = p->next;
        q++;
    }

    return ids;
}

/********************************************************************
 * compare_arrays_int: Checks if the first length elements of two
 *                     int-arrays are equal and in the same order.
 ********************************************************************/
bool compare_arrays_int(int *ar_1, int *ar_2, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (*ar_1++ != *ar_2++)
        {
            return false;
        }
    }

    return true;
}

/********************************************************************
 * compare_sorted_arrays_int: Checks if the first length elements of
 *                            two int-arrays are equal
 *                            (in any order).
 ********************************************************************/
bool compare_sorted_arrays_int(int *ar_1, int *ar_2, int length)
{
    int i;
    int *p;

    // preparing first array
    int *dup_1 = malloc(length * sizeof(*dup_1));
    MEM_TEST(dup_1);

    p = dup_1;
    for (i = 0; i < length; i++)
        *p++ = *ar_1++;
    
    qsort(dup_1, length, sizeof(int), compare_ints);

    // preparing second array
    int *dup_2 = malloc(length * sizeof(*dup_2));
    MEM_TEST(dup_2);

    p = dup_2;
    for (i = 0; i < length; i++)
        *p++ = *ar_2++;

    qsort(dup_2, length, sizeof(int), compare_ints);

    // comparing arrays
    p = dup_1;
    int *q = dup_2;
    for (i = 0; i < length; i++)
    {
        if (*p++ != *q++)
        {
            free(dup_1);
            free(dup_2);
            return false;
        }
    }

    free(dup_1);
    free(dup_2);
    return true;
}

/********************************************************************
 * compare_ints: Used in compare_sorted_arrays_int() for qsort.
 ********************************************************************/
PRIVATE int compare_ints(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;
    if (x < y)
        return -1;
    if (x > y)
        return 1;
    return 0;
}
