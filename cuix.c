#include <stdlib.h>
#include "cuix.h"

object *
create_menu (char *title)
{
    return NULL;
}



entry *
create_menuentry (char *label)
{
    return NULL;
}



void 
add_submenu (object *father, object *child)
{
}


void 
add_entry (object *father, entry *child)
{
}


object *
create_form (char *title)
{
    return NULL;
}


entry *
create_label (char *label)
{
    return NULL;
}


/* result must be allocated to >= MAX_FIELD_SIZE bytes */
entry *
create_field (char *label, void **result)
{
    return NULL;
}


void 
add_label (object *father, entry *child)
{
}



void 
add_field (object *father, entry *child)
{
}

/* entries must be NULL terminated */
object *
create_list (char *title, entry **entries)
{
    return NULL;
}

void
display_object (object *obj)
{
}


int
main ()
{
    return 0;
}
