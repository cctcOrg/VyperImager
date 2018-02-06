#ifndef DIALOGS_H
#define DIALOGS_H

#include <gtk/gtk.h>
#include "appdefs.h"

/* This dialog will simply display the given message and present an OK button.
 * Good for alerting the user of information, or for debugging */
GtkWidget *create_alert_dialog(char *msg);

/* Will alert the user that no device was selected (i.e. in a GtkTreeView) */
GtkWidget *create_no_device_dialog(GtkWidget *window, char *type);

/* Will ask the user if they really want to select the target given by the target
 * parameter as the target device. Provides a cancel button in case that is not
 * the device the user is looking for */
GtkWidget *create_confirm_target_device_dialog(GtkWidget *window, char *target);

/* A dialog to be run if the user has not selected an OS combination to
 * determine the filesystem to format the target as */
GtkWidget *create_please_choose_system_dialog(GtkWidget *window);

/* Creates a simple dialog with a progress spinner. Will set globals->dialog_box
 * to its content area so that it can be updated as a process completes */
GtkWidget *create_progress_spinner_dialog(GtkWidget *window, app_objects *globals);

GtkWidget *create_progress_bar_dialog(GtkWidget *window, app_objects *globals);

#endif /* DIALOGS_H */
