#include <gtk/gtk.h>

GtkWidget *create_alert_dialog(char *msg);
GtkWidget *create_no_device_dialog(GtkWidget *window, char *type);
GtkWidget *create_confirm_target_device_dialog(GtkWidget *window, char *target);
GtkWidget *create_same_device_dialog(GtkWidget *window);
GtkWidget *create_please_choose_system_dialog(GtkWidget *window);
