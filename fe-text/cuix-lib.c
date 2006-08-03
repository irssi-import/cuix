#include "module.h"
#include "settings.h"
#include "cuix-lib.h"
#include "signals.h"
#include "gui-windows.h"


int do_nothing (char *foo)
{
    (void)foo;
    return 0;
}

int change_nick (char *nick)
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
    signal_emit("command nick", 3, nick, server, wiitem);
    return 0;
}




int change_nick_form (char *nothing) {
    object *form;
    entry *question, *answer;
    (void)nothing;

    form = create_form ("True!");
    question = create_label ("Enter your new nick");
    answer = create_field ("", change_nick);
    attach_entry (form, question);
    attach_entry (form, answer);
    display_object (form);
    return 0;
}


int about_list (char *nothing) 
{
    object *list;
    entry *about, *entries[2];
    (void)nothing;

    about = create_label ("(c) irssi; See http://www.irssi.org.");
    entries[0] = about;
    entries[1] = NULL;
    list = create_list ("about", entries);
    attach_entry (list, about);
    display_object (list);
    return 0;
}




int home_menu (char *nothing) 
{
    /* Objects declaration */
    object *root_menu, *submenu;
    entry *label11, *label21, *label22;
    (void)nothing;

    /* Objects initialisation */
    root_menu = create_menu ("My root menu");
    submenu = create_menu (" My submenu");
    label11 = create_menuentry ("Change nick", change_nick_form);
    label21 = create_menuentry ("About", about_list);
    label22 = create_menuentry ("", do_nothing);

    /* Layout */
    attach_submenu (root_menu, submenu);
    attach_entry (root_menu, (void *)label11);
    attach_entry (root_menu, (void *)label21);
    attach_entry (submenu, (void *)label22);

    /* Declare that the object is ready to be displayed and do it */
    display_object (root_menu);
    return 0;
}
