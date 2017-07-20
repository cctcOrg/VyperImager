#ifndef CALLBACKS_H
#define CALLBACKS_H

#include<gtk/gtk.h>

void notebook_previous_page(GtkWidget *w, gpointer udata);
void check_tv_and_next(GtkWidget *w, gpointer udata);
void get_target_info_and_next(GtkWidget *w, gpointer udata);

#endif /* CALLBACKS_H */
