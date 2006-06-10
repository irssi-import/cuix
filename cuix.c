#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>
#include <panel.h>
#include "cuix.h"

#define INIT_ENTRIES 8
#define X0_OFFSET 2
#define Y0_OFFSET 2
#define Y_OFFSET 1

object *
create_object (char *title, int type, void **entries)
{
    object *obj;
    void **new_entries;
    int i;

    obj = malloc (sizeof(object));
    if (!obj)
    {
        return NULL;
    }
    obj->type = type;
    obj->title = title;
    if (!entries)
    {
        new_entries = calloc (INIT_ENTRIES, sizeof(void *));
        obj->entries = new_entries;
        obj->alloced = INIT_ENTRIES;
        obj->last = 0;
    } else {
        for (i = 0; ((entry **)entries)[i]; i++);
        obj->alloced = i - 1;
        obj->last = i - 1;
        obj->entries = entries;
    }
    return obj;
}


object *
create_menu (char *title)
{
    return create_object (title, MENU, NULL);
}


object *
create_form (char *title)
{
    return create_object (title, FORM, NULL);
}


/* entries must be NULL terminated */
object *
create_list (char *title, entry **entries)
{
    return create_object (title, LIST, (void **)entries);
}

entry *
create_entry (char *label, int type, action_fn_type action)
{
    entry *entry;
    
    entry = malloc (sizeof(object));
    if (!entry)
    {
        return NULL;
    }
    entry->type = type;
    entry->data = label;
    entry->action = action;
    return entry;
}

entry *
create_menuentry (char *label, action_fn_type action)
{
    return create_entry (label, MENUENTRY, action);
}

entry *
create_label (char *label)
{
    return create_entry (label, LABEL, NULL);
}


entry *
create_field (char *label, action_fn_type action)
{
    return create_entry (label, FIELD, action);
}



/* Adds child at the last position of father->entries */
void 
attach_entry (object *father, void *child)
{
    void **entries;
    int i;

    /* Check that we have enough space in father->entries, otherwise alloc
     * twice more than previously */
    if (father->last >= father->alloced)
    {
        entries = calloc (2 * father->alloced, sizeof(void *));
        if (!entries)
        {
            fprintf (stderr, "Problem with memory allocation, quitting now...\n");
            exit (1);
        }
        for (i = 0; i < father->alloced; i++)
        {
            entries[i] = father->entries[i];
        }
        free (father->entries);
        father->entries = entries;
        father->alloced *= 2;
    }

    father->entries[father->last++] = child;
}


/* Adds a submenu to father */
void
attach_submenu (object *father, object *child)
{ 

    /* Check that both are really menus */
    if (father->type != MENU || child->type != MENU)
    {
        fprintf (stderr, "Typing error, trying to add %p (%d) as child of"
                "%p (%d)\n", father, father->type, child, child->type);
        exit (1);
    }
    attach_entry (father, (void *)child);
}


/* Returns the maximum width occupied by labels */
int
get_labels_width (object *obj)
{
    int i;
    unsigned int w = 0;
    entry *e;

    for (i = 0; i < obj->last; i++)
    {
        e = (entry *)obj->entries[i];
        if (e->type == LABEL)
        {
            w = (w > strlen (e->data)) ? w : strlen (e->data);
        }
    }
    w += 2 * X0_OFFSET;
    return (int)w;
}


/* Puts in x and y the coordinates to center an object of size objw and objh
 * in the window win */
void
get_center (WINDOW *win, int objw, int objh, int *x, int *y)
{
    int begx, begy, maxx, maxy, w, h;
    getbegyx (win, begy, begx);
    getmaxyx (win, maxy, maxx);
    w = maxx - begx;
    h = maxy - begy;
    *x = (w - objw) / 2 + begx;
    *y = (h - objh) / 2 + begy;
}



void
display_object (object *obj)
{
    WINDOW *win;
    entry *e;
    int i, x, y, w, h;

    if (obj->type >= LABEL)
    {
        fprintf (stderr, "Trying to display an entry %p (%d), terminating...\n",
                obj, obj->type);
        exit (1);
    }

    switch (obj->type)
    {
        case LIST:
            w = get_labels_width (obj);
            h = Y_OFFSET * obj->last + 2 * Y0_OFFSET + 1;
            get_center (stdscr, w, h, &x, &y);
            win = newwin (h, w, y, x);
            box (win, 0, 0);
            x = X0_OFFSET;
            y = Y0_OFFSET;

            for (i = 0; i < obj->last; i++)
            {
                e = (entry *)obj->entries[i];
                if (e->type != LABEL)
                {
                    fprintf (stderr, "Non-label entry in a list.\n");
                    exit (1);
                } 
                wmove (win,y,x);
                waddstr (win,e->data); 
                y += Y_OFFSET;
                x = X0_OFFSET;
            }
            break;
        default:
            break;
    }
    /* update_panels (); */
    /* doupdate (); */
}

int
do_nothing (char *foo)
{
    (void)foo;
    return 0;
}

void
my_menu (void) {

    /* Objects declaration */
    object *root_menu, *submenu;
    entry *label11, *label21, *label22;

    /* Objects initialisation */
    root_menu = create_menu ("My root menu");
    submenu = create_menu ("My submenu");
    label11 = create_menuentry ("Sweden", do_nothing);
    label21 = create_menuentry ("is a", do_nothing);
    label22 = create_menuentry ("beautiful country", do_nothing);

    /* Layout */
    attach_submenu (root_menu, submenu);
    attach_entry (root_menu, (void *)label11);
    attach_entry (submenu, (void *)label21);
    attach_entry (submenu, (void *)label22);

    /* Declare that the object is ready to be displayed and do it */
    display_object (root_menu);
}

void 
my_list (void) {
    object *list;
    entry *bli1, *bli2;
    list = create_list ("True!", NULL);
    bli1 = create_label ("Sweden");
    bli2 = create_label ("rulez!");
    attach_entry (list, bli1);
    attach_entry (list, bli2);
    display_object (list);
}


/* This is only because we are compiling cuix outside of irssi so we don't
 * have access to all the window objects and things */
void
init_stuff (void)
{
    initscr ();
    keypad (stdscr, TRUE);
    nonl ();
    cbreak ();
}


int
main ()
{
    init_stuff ();
    printf ("bli\n");
    my_list ();
    printf ("prout\n");
    (void)getch ();
    endwin ();

    return 0;
}
