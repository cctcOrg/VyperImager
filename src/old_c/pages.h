#ifndef PAGES_H
#define PAGES_H

#include<gtk/gtk.h>
#include "appdefs.h"

/* This simply will create and set a model for a TreeView. You can call this
 * function again with globals->ttv and globals->etv to reload */
void set_treeview_model(GtkWidget *treeview, int hide_internal, app_objects *globals);
GtkWidget *create_welcome_box(app_objects *globals);
GtkWidget *create_target_selector(app_objects *globals);
GtkWidget *create_evidence_selector(app_objects *globals);
GtkWidget *create_format_selector(app_objects *globals);
GtkWidget *create_target_interface(app_objects *globals);
GtkWidget *create_case_metadata_interface(app_objects *globals);
GtkWidget *create_image_metadata_interface(app_objects *globals);
GtkWidget *create_summary_interface(app_objects *globals);

#endif /* PAGES_H */
