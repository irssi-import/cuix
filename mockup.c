/* Here is what a typical use will look like: */

#include "cuix.h"

void
my_menu (void) {

    /* Objects declaration */
    object *root_menu, *submenu;
    entry *label11, *label12, *label21;

    /* Objects initialisation */
    root_menu = create_menu ("My root menu");
    submenu = create_menu ("My submenu");
    label11 = create_menuentry ("Sweden");
    label21 = create_menuentry ("is a");
    label22 = create_menuentry ("beautiful country");

    /* Layout */
    add_submenu (root_menu, submenu);
    attach_entry (root_menu, label11);
    attach_entry (submenu, label21);
    attach_entry (submenu, label22);

    /* Declare that the object is ready to be displayed and do it */
    display (root_menu);
}


char *
my_form (void) {
    object *my_form;
    entry *question, *hint;
    char *answer;

    answer = malloc (MAX_FIELD_SIZE * sizeof(char));
    my_form = create_form ("Really important question");
    question = create_field ("What is the most beautiful country in the world?");
    hint = create_label ("(Hint: it's between Norway and Finland)", answer);

    attach_entry (my_form, question);
    attach_entry (my_form, hint);

    /* XXX: should this be blocking? When can we safely use answer? */
    display (my_form);

    return answer;
}
    

