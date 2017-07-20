#ifndef PAGES_H
#define PAGES_H

#include<gtk/gtk.h>
#include "appdefs.h"

void set_box_margins(GtkWidget *w);
GtkWidget *create_welcome_box(app_objects *globals);
GtkTreeModel *create_block_devices_liststore(int hide_internal);
GtkWidget *create_block_devices_treeview(int hide_internal);
GtkWidget *create_target_interface(app_objects *globals);

#endif /* PAGES_H */
