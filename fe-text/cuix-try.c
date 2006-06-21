#include "module.h"
#include "settings.h"
#include "cuix-try.h"
#include "term.h"
#if defined(USE_NCURSES) && !defined(RENAMED_NCURSES)
#  include <ncurses.h>
#else
#  include <curses.h>
#endif

/* Should be updated if the one in term-curses.c changes */
struct _TERM_WINDOW {
	int x, y;
        int width, height;
	WINDOW *win;
};

WINDOW *cuix_win;

void cuix_try(void)
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
    my_list ();
}


void cuix_quit(void)
{
    delwin (cuix_win);
    wrefresh (root_window->win);
}
