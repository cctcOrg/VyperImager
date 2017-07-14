#include<gtk/gtk.h>
#include "devinfo.h"
#include "image_types.h"
#include "dialogs.h"

typedef struct window_data {
    GtkWidget *ttv;
    GtkWidget *etv;
    /* Need to pass these on since the first page will be hidden/destroyed */
    GtkWidget *app_box;
    GtkWidget *window;
} tv_data;

GtkWidget *create_welcome_box(GtkWidget *window);
static void activate(GtkApplication *app, gpointer user_data);
GtkWidget *create_block_devices_treeview();
void check_tv_and_next(GtkWidget *w, gpointer udata);
void create_target_interface(GtkWidget *window, ImageInfo *info);

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

GtkWidget *create_welcome_box(GtkWidget *window) {
    GtkWidget *app_box;
    GtkWidget *button_box;
    GtkWidget *button;
    GtkWidget *evid_device_tv;
    GtkWidget *targ_device_tv;

    tv_data *udata = malloc(sizeof(tv_data*));

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_EDGE);

    gtk_widget_set_margin_start(button_box, 30);
    gtk_widget_set_margin_end(button_box, 30);
    gtk_widget_set_margin_bottom(button_box, 50);
    gtk_widget_set_size_request(button_box, -1, 100);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Welcome to the CCTC Imaging Toolkit!"),
                TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the device to image"),
                TRUE, TRUE, 10);

    evid_device_tv = create_block_devices_treeview(SHOW_INT);
    gtk_box_pack_start(GTK_BOX(app_box), evid_device_tv, TRUE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the target device"),
                TRUE, TRUE, 20);

    targ_device_tv = create_block_devices_treeview(HIDE_INT);
    gtk_box_pack_start(GTK_BOX(app_box), targ_device_tv, TRUE, TRUE, 10);

    button = gtk_button_new_with_label("Utilities");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    udata->etv = evid_device_tv;
    udata->ttv = targ_device_tv;
    udata->app_box = app_box;
    udata->window = window;

    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(check_tv_and_next), udata);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), window);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_container_add(GTK_CONTAINER(app_box), button_box);
    
    return app_box;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *app_box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "CCTC Imaging ToolKit");
    gtk_window_fullscreen(GTK_WINDOW(window));

    app_box = create_welcome_box(window);

    gtk_container_add(GTK_CONTAINER(window), app_box);
    gtk_widget_show_all(window);
}

GtkTreeModel *create_block_devices_liststore(int hide_internal) {
    GtkListStore  *store;
    GtkTreeIter    iter;

    int num_blockdevs;
    Device **blockdev_info;

    /* Device, model, size */
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, 
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    blockdev_info = get_blockdev_info(&num_blockdevs);

    for (int i=0; i<num_blockdevs; i++) {
        if (hide_internal)
            if (!blockdev_info[i]->removable)
                continue;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                COL_DEV, blockdev_info[i]->name,
                COL_MODEL, blockdev_info[i]->model,
                COL_SIZE, blockdev_info[i]->size,
                COL_REMOVABLE, (blockdev_info[i]->removable) ? "Yes" : "No",
                -1);
    }
    return GTK_TREE_MODEL(store);
}

GtkWidget *create_block_devices_treeview(int hide_internal) {
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    GtkWidget           *treeview;
    
    treeview = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Device",  
            renderer,
            "text", COL_DEV,
            NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Model",  
            renderer,
            "text", COL_MODEL,
            NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Size",  
            renderer,
            "text", COL_SIZE,
            NULL);

    if (!hide_internal) {
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
                -1,      
                "Removable",  
                renderer,
                "text", COL_REMOVABLE,
                NULL);
    }

    model = create_block_devices_liststore(hide_internal);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);

    return treeview;
}

void check_tv_and_next(GtkWidget *w, gpointer udata) {
    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *diag;
    
    char *name;
    ImageInfo *info = malloc(sizeof(ImageInfo*));

    tv_data *data = udata;

    /* Get evidence device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->etv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(4*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->evidence_device = name;
    } 
    else {
        diag = create_no_device_dialog(data->window, "evidence");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }

    /* Get target device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->ttv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(4*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->target_device = name;
    } 
    else {
        diag = create_no_device_dialog(data->window, "target");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }

    gtk_widget_destroy(data->app_box);
    create_target_interface(data->window, info);
}

void create_target_interface(GtkWidget *window, ImageInfo *info) {
    gtk_widget_destroy(window);
}
