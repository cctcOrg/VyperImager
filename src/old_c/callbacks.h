#ifndef CALLBACKS_H
#define CALLBACKS_H

#include<gtk/gtk.h>

/* Many callbacks use this signature, so it made sense to make it a macro */
#define NEW_CALLBACK(name) void name(GtkWidget *w, gpointer udata)

/* Gets the previous page from the stack, navigate the notebook to
 * it, and then get that page's title to set the header bar title */
NEW_CALLBACK(notebook_previous_page);
/* Unmounts the target drive and sets the evidence device back to RW, and then
 * quits the app */
NEW_CALLBACK(quit_app);

NEW_CALLBACK(welcome_page_cb);
NEW_CALLBACK(target_device_cb);
NEW_CALLBACK(evidence_device_cb);
NEW_CALLBACK(format_device_cb);
NEW_CALLBACK(get_target_info_cb);
NEW_CALLBACK(case_info_cb);
NEW_CALLBACK(image_info_cb);
NEW_CALLBACK(create_image_cb);

#endif /* CALLBACKS_H */
