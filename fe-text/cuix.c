#include "module.h"
#include "settings.h"
#include "cuix_api.h"
#include "cuix.h"
#include "term.h"
#if defined(USE_NCURSES) && !defined(RENAMED_NCURSES)
#  include <ncurses.h>
#else
#  include <curses.h>
#endif

int cuix;

void cuix_create(void)
{
    /* 
    WINDOW *rw = root_window->win;
    cuix_win = newwin (10, 30, 2, 2);
    box (cuix_win, 0, 0);
    wmove (cuix_win, 1, 1);
    waddstr (cuix_win, "Enter your new nick");
    wrefresh (rw);
    wrefresh (cuix_win);
    */
    my_form ();
}

void
cuix_refresh (void)
{
    /* wrefresh (root_window->win); */
    /* if (cuix_win) */
        /* wrefresh (cuix_win); */
    update_panels ();
}

void
cuix_destroy (void)
{
    if (cuix_win)
    {
        del_panel (p_cuix);
        delwin(cuix_win);
    }
    cuix_win = NULL;
    term_refresh (root_window);
}
