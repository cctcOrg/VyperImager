#include<gtk/gtk.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "dialogs.h"

GtkWidget *create_alert_dialog(char *msg) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Info", NULL, flags,
                                          "OK", GTK_RESPONSE_ACCEPT, NULL);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(dialog_area), gtk_label_new(msg));
    
    gtk_widget_show_all(dialog);
    return dialog;
}

GtkWidget *create_no_device_dialog(GtkWidget *window, char *type) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    char *msg_format = "A %s device must be selected";
    char *msg = malloc((strlen(msg_format)+strlen(type)+1)*sizeof(char));
    sprintf(msg, msg_format, type);

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Error!", GTK_WINDOW(window), flags,
                                          "OK", GTK_RESPONSE_ACCEPT, NULL);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(dialog_area), gtk_label_new(msg));
    free(msg);
    
    gtk_widget_show_all(dialog);
    return dialog;
}

GtkWidget *create_confirm_target_device_dialog(GtkWidget *window, char *target) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    char *msg_format = ("All data on /dev/%s will be lost, are you "
            "sure you wish to continue?");
    char *msg = malloc((strlen(msg_format)+strlen(target)+1)*sizeof(char));
    sprintf(msg, msg_format, target);

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Confirm", GTK_WINDOW(window), flags,
                                          "OK", GTK_RESPONSE_ACCEPT, 
                                          "Cancel", GTK_RESPONSE_CANCEL, NULL);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(dialog_area), gtk_label_new(msg));
    free(msg);
    
    gtk_widget_show_all(dialog);
    return dialog;
}

GtkWidget *create_please_choose_system_dialog(GtkWidget *window) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    char *msg = "Please select which systems the target drive should be readable by."; 

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Error!", GTK_WINDOW(window), flags,
                                          "OK", GTK_RESPONSE_ACCEPT, NULL);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(dialog_area), gtk_label_new(msg));
    
    gtk_widget_show_all(dialog);
    return dialog;
}

GtkWidget *create_progress_spinner_dialog(GtkWidget *window, app_objects *globals) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;
    GtkWidget *spinner;

    spinner = gtk_spinner_new();
    globals->spinner = spinner;

    char *msg = "Working..."; 

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Loading", GTK_WINDOW(window), flags,
                                          "OK", GTK_RESPONSE_ACCEPT, NULL);
    gtk_widget_set_size_request(dialog, 200, 200);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    globals->dialog_box = dialog_area;
    gtk_box_pack_start(GTK_BOX(dialog_area), gtk_label_new(msg), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(dialog_area), spinner, TRUE, TRUE, 30);

    gtk_widget_set_margin_start(dialog_area, 30); 
    gtk_widget_set_margin_end(dialog_area, 30); 
    gtk_widget_set_margin_top(dialog_area, 30); 
    gtk_widget_set_margin_bottom(dialog_area, 30); 

    gtk_spinner_start(GTK_SPINNER(spinner));
    
    gtk_widget_show_all(dialog);

    return dialog;
}
