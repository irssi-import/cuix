#include "module.h"
#include "settings.h"
#include "term.h"
#include "signals.h"
#include "gui-windows.h"
#include <stdarg.h>
#if defined(USE_NCURSES) && !defined(RENAMED_NCURSES)
#  include <ncurses.h>
#else
#  include <curses.h>
#endif
#include <form.h>
#include <panel.h>


#include "cuix_api.h"

#define INIT_ENTRIES 8
#define X0_OFFSET 4
#define Y0_OFFSET 2
#define Y_OFFSET 1
#define CUIX_FIELD_WIDTH 16

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
    return create_object (title, CUIX_MENU, NULL);
}


object *
create_form (char *title)
{
    return create_object (title, CUIX_FORM, NULL);
}


/* entries must be NULL terminated */
object *
create_list (char *title, entry **entries)
{
    return create_object (title, CUIX_LIST, (void **)entries);
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
    return create_entry (label, CUIX_MENUENTRY, action);
}

entry *
create_label (char *label)
{
    return create_entry (label, CUIX_LABEL, NULL);
}


entry *
create_field (char *label, action_fn_type action)
{
    return create_entry (label, CUIX_FIELD, action);
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
    if (father->type != CUIX_MENU || child->type != CUIX_MENU)
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
        if (e->type == CUIX_LABEL)
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
get_center (WINDOW *win, int objh, int objw, int *y, int *x)
{
    int begx, begy, maxx, maxy, w, h;
    getbegyx (win, begy, begx);
    getmaxyx (win, maxy, maxx);
    w = maxx - begx;
    h = maxy - begy;
    *x = (w - objw) / 2 + begx;
    *y = (h - objh) / 2 + begy;
    if (*x < 0)
        *x = 0;
    if (*y < 0)
        *y = 0;
}



void
display_object (object *obj)
{
    FORM *form;
    FIELD **fields;
    entry *e;
    char *result;
    int i, x, y, w, h;
    int ch;
    p_main = new_panel(root_window->win);

    if (obj->type >= CUIX_LABEL)
    {
        fprintf (stderr, "Trying to display an entry %p (%d), terminating...\n",
                obj, obj->type);
        exit (1);
    }

    switch (obj->type)
    {
        case CUIX_LIST:
            w = get_labels_width (obj);
            h = Y_OFFSET * obj->last + 2 * Y0_OFFSET;
            get_center (root_window->win, h, w, &y, &x);
            cuix_win = newwin (h, w, y, x);
            box (cuix_win, 0, 0);
            p_cuix = new_panel(cuix_win);
            x = X0_OFFSET;
            y = Y0_OFFSET;

            for (i = 0; i < obj->last; i++)
            {
                e = (entry *)obj->entries[i];
                if (e->type != CUIX_LABEL)
                {
                    fprintf (stderr, "Non-label entry in a list.\n");
                    exit (1);
                } 
                wmove (cuix_win,y,x);
                waddstr (cuix_win,e->data); 
                y += Y_OFFSET;
                x = X0_OFFSET;
            }
            top_panel (p_cuix);
            update_panels();
            doupdate();
            /* refresh (); */
            /* wrefresh (cuix_win); */
            break;
        case CUIX_FORM:
            w = get_labels_width (obj);
            w = (w > CUIX_FIELD_WIDTH + 2 * X0_OFFSET) ? w : CUIX_FIELD_WIDTH + 2 * X0_OFFSET;
            h = Y_OFFSET * obj->last + 2 * Y0_OFFSET;
            fields = calloc (obj->last + 1, sizeof(FIELD *));
            for (i = 0; i < obj->last; i++) 
            {
                e = (entry *)obj->entries[i];
                fields[i] = new_field (1, w, Y0_OFFSET + i * Y_OFFSET, X0_OFFSET, 0, 0);
                if (e->type == CUIX_LABEL)
                {
                    field_opts_off (fields[i], O_ACTIVE);
                    field_opts_off (fields[i], O_EDIT);
                    set_field_back  (fields[i], A_BOLD);
                }
                set_field_buffer (fields[i], 0, e->data);
            }
            fields[obj->last] = NULL;
            form = new_form (fields);
            scale_form (form, &h, &w);
            h += Y0_OFFSET;
            w += 2 * X0_OFFSET;
            get_center (root_window->win, h, w, &y, &x);
            cuix_win = newwin (h, w, y, x);
            keypad (cuix_win, TRUE);
            nonl ();
            /* win = root_window->win; */
            set_form_win (form, cuix_win);
            set_form_sub (form, derwin(cuix_win, w, h, X0_OFFSET, Y0_OFFSET));
            post_form (form);
            box (cuix_win, 0, 0);
            p_cuix = new_panel (cuix_win);
            top_panel (p_cuix);
            /* refresh (); */
            /* wrefresh (cuix_win); */
            while((ch = wgetch(cuix_win)) != '\n' && ch != '\r' && ch != 27 /* ESC */)
            {       
                /* fprintf (stderr,"pressed %d\n", ch); */
                switch(ch)
                {       
                    case KEY_DOWN:
                        /* Go to next field */
                        form_driver(form, REQ_NEXT_FIELD);
                        /* Go to the end of the present buffer */
                        /* Leaves nicely at the last character */
                        form_driver(form, REQ_END_LINE);
                        break;
                    case KEY_UP:
                        /* Go to previous field */
                        form_driver(form, REQ_PREV_FIELD);
                        form_driver(form, REQ_END_LINE);
                        break;
                    case KEY_BACKSPACE:
                        form_driver(form, REQ_PREV_CHAR);
                        form_driver(form, REQ_DEL_CHAR);
                        break;
                    case KEY_LEFT:
                        form_driver(form, REQ_PREV_CHAR);
                        break;
                    case KEY_RIGHT:
                        form_driver(form, REQ_NEXT_CHAR);
                        break;
                    default:
                        /* If this is a normal character, it gets */
                        /* Printed                                */    
                        form_driver(form, ch);
                        break;
                }
            }
            form_driver (form, REQ_VALIDATION);
            for (i = 0; i < obj->last; i++) 
            {
                e = (entry *)obj->entries[i];
                if (e->type == CUIX_FIELD)
                {
                    result = field_buffer(fields[i],0);
                    e->action (result);
                }
            }
            unpost_form (form);
            /* del_panel (p_cuix); */
            /* delwin (cuix_win); */
            /* cuix_win = NULL; */
            /* p_cuix = NULL; */
            /* update_panels (); */
            /* doupdate (); */

            break;
        default:
            break;
    }
}



/* Usage: irssi_command(server, witem, cmd, arg1, arg2, argn, NULL) */
void irssi_command(SERVER_REC *server, WI_ITEM_REC *witem, const char *cmd, ...)
{
    GString *gstr;
    char *arg; 
    char *cmdargs, *cmdname;
    va_list ap;

    gstr = g_string_new("");
    
    va_start(ap, cmd);
    while ((arg = va_arg(ap, char *)) != NULL)
    { 
        g_string_append(gstr, arg); 
        g_string_append_c(gstr, ' ');
    }
    va_end(ap);

    cmdargs = g_string_free(gstr, FALSE);
    cmdname = g_strconcat("NICK ", cmd, NULL);

    signal_emit(cmdname, 3, cmdargs, server, witem);
  
    g_free(cmdname);
    g_free(cmdargs);
}

    int
do_nothing (char *foo)
{
    (void)foo;
    return 0;
}

int
change_nick (char *nick)
{
    SERVER_REC *server;
    WI_ITEM_REC *wiitem;
    if (active_win == NULL) {
        server = NULL;
        wiitem = NULL;
    } else {
        server = active_win->active_server != NULL ?
            active_win->active_server : active_win->connect_server;
        wiitem = active_win->active;
    } 
    fprintf (stderr, "nick: %s, %d\n", nick, strlen(nick));
    signal_emit("command nick", 3, nick, server, wiitem);
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

void 
my_form (void) {
    object *list;
    entry *bli1, *bli2;
    list = create_form ("True!");
    bli1 = create_label ("Enter your new nick");
    bli2 = create_field ("", change_nick);
    attach_entry (list, bli1);
    attach_entry (list, bli2);
    display_object (list);
}


