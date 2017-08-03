#ifndef CALLBACKS_H
#define CALLBACKS_H

#include<gtk/gtk.h>

#define NEW_CALLBACK(name) void name(GtkWidget *w, gpointer udata)

NEW_CALLBACK(notebook_previous_page);

NEW_CALLBACK(check_tv_cb);
NEW_CALLBACK(format_device_cb);
NEW_CALLBACK(get_target_info_cb);
NEW_CALLBACK(case_info_cb);

#endif /* CALLBACKS_H */
