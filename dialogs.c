#include<gtk/gtk.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

GtkWidget *create_no_device_dialog(GtkWidget *window, char *type) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    char *msg_format = "A %s device must be selected";
    char *msg = malloc((strlen(msg_format) + strlen(type) +1)*sizeof(char));
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
    char *msg = malloc((strlen(msg_format) + strlen(target))*sizeof(char));
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

GtkWidget *create_same_device_dialog(GtkWidget *window) {
    GtkWidget *dialog;
    GtkWidget *dialog_area;

    char *msg = "The evidence and target device cannot be the same."; 

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons("Error!", GTK_WINDOW(window), flags,
                                          "OK", GTK_RESPONSE_ACCEPT, NULL);

    dialog_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(dialog_area), gtk_label_new(msg));
    
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
