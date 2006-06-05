#define MAX_FIELD_SIZE 64

enum objtype {
    MENU,
    FORM,
    LIST,
};

enum entrytype {
    LABEL,
    FIELD,
    SUBMENU,
};

typedef struct entry {
    int type;
    char *data; /* contains label or submenu title */
    void **result; /* in case of a field, store result here */
} entry;


typedef struct object {
    int type;
    char *title;
    void **entries; /* must be NULL terminated */
} object;



/* Object definitions */

object *create_menu (char *title);
object *create_form (char *title);
/* entries must be NULL terminated */
object *create_list (char *title, entry **entries);


entry *create_menuentry (char *label);
entry *create_label (char *label);
/* result must be allocated to >= MAX_FIELD_SIZE bytes */
entry *create_field (char *label, void **result);

void attach_submenu (object *father, object *child);
void attach_entry (object *father, entry *child);

void display_object (object *obj);
