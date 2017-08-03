#include<stdlib.h>
#include<string.h>
#include "callbacks.h"
#include "appdefs.h"
#include "dialogs.h"

static void set_next_hb_title(app_objects *g) {
    int current_page;
    GtkWidget *current_child;
    const char *tab_label;
    GtkWidget *nb = g->notebook;

    current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
    current_child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), current_page);
    tab_label = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(nb), current_child);
    gtk_header_bar_set_title(GTK_HEADER_BAR(g->header), tab_label);
}

NEW_CALLBACK(notebook_previous_page) {
    (void) w;
    app_objects *globals = udata;
    GtkWidget *nb = globals->notebook;
    
    int current_page;
    GtkWidget *current_child;
    const char *tab_label;
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), globals->prev_page);
    current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
    current_child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), current_page);
    tab_label = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(nb), current_child);
    gtk_header_bar_set_title(GTK_HEADER_BAR(globals->header), tab_label);
}

NEW_CALLBACK(check_tv_cb) {
    (void) w;
    app_objects *globals = udata;
    GtkWidget *window = globals->window;
    ImageInfo *info = globals->user_info;
    

    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *diag;
    
    char *name;

    /* Get evidence device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(globals->etv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(4*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->evidence_device = name;
    } 
    else {
        diag = create_no_device_dialog(window, "evidence");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }

    /* Get target device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(globals->ttv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(4*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->target_device = name;
    } 
    else {
        diag = create_no_device_dialog(window, "target");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }
    
    if (strcmp(info->evidence_device, info->target_device) == 0) {
        diag = create_same_device_dialog(window);
        gtk_dialog_run(GTK_DIALOG(diag));
        return;
    }

    globals->prev_page = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(globals->format_dev)))
        gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    /* If you don't need to format skip to page 2 */
    else
        gtk_notebook_set_current_page(GTK_NOTEBOOK(globals->notebook), 2);

    set_next_hb_title(globals);

}

NEW_CALLBACK(format_device_cb) {
    (void) w;
    GtkWidget *diag;

    app_objects *globals = udata;
    GtkWidget *window = globals->window;
    ImageInfo *info = globals->user_info;

    char buttons_state = 0;
    char *fs_choice = malloc(7*sizeof(char));

    for (int i=0; i<3; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(globals->os_buttons[i])))
            buttons_state |= 1<<i;
    }
    
    switch (buttons_state) {
        /* Windows, Mac, Linux */
        case 0b011:
        case 0b111:
        case 0b110:
            strcpy(fs_choice, "vfat");
            break;
        /* Mac, Linux */
        case 0b100:
        case 0b101:
            strcpy(fs_choice, "hfsplus");
            break;
        /* Windows */
        case 0b010:
            strcpy(fs_choice, "ntfs");
            break;
        /* Linux */
        case 0b001:
            strcpy(fs_choice, "ext4");
            break;
        default:
            diag = create_please_choose_system_dialog(globals->window);
            gtk_dialog_run(GTK_DIALOG(diag));
            gtk_widget_destroy(diag);
            return;
    }

    globals->user_info->target_filesystem = fs_choice; 
    printf("User has requested filesystem %s\n", fs_choice);

    diag = create_confirm_target_device_dialog(window, info->target_device);
    int result = gtk_dialog_run(GTK_DIALOG(diag));
    gtk_widget_destroy(diag);
    if (result != GTK_RESPONSE_ACCEPT)
        return;

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));

    set_next_hb_title(globals);
}

NEW_CALLBACK(get_target_info_cb) {
    (void) w;
    app_objects *globals = udata;

    GtkWidget *diag;
    GtkWidget *entry;

    char *entry_data;
    const char *entry_tmp;
    
    entry = globals->filename_entry;

    entry_tmp = gtk_entry_get_text(GTK_ENTRY(globals->filename_entry));
    entry_data = malloc((strlen(entry_tmp)+1)*sizeof(char));
    strcpy(entry_data, entry_tmp);
    printf("%s\n", entry_data);

    free(entry_data);

    /*gtk_notebook_next_page(GTK_NOTEBOOK(g_OBJS.notebook));*/
}

