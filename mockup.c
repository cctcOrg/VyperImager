#include<gtk/gtk.h>
#include "devinfo.h"

enum {
    COL_DEV = 0,
    COL_MODEL,
    COL_SIZE,
    COL_REMOVABLE,
    NUM_COLS
};

GtkWidget *create_welcome_box(GtkWidget *window);
static void activate(GtkApplication *app, gpointer user_data);
GtkWidget *create_block_devices_treeview();

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

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_EXPAND);

    gtk_widget_set_margin_start(button_box, 30);
    gtk_widget_set_margin_end(button_box, 30);
    gtk_widget_set_margin_bottom(button_box, 50);
    gtk_widget_set_size_request(button_box, -1, 200);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Welcome to the CCTC Imaging Toolkit!"),
                TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the device to image"),
                TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), create_block_devices_treeview(SHOW_INT),
            TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the target device"),
                TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), create_block_devices_treeview(HIDE_INT),
            TRUE, TRUE, 30);

    button = gtk_button_new_with_label("List");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(create_block_devices_treeview), NULL);

    button = gtk_button_new_with_label("Image");
    gtk_container_add(GTK_CONTAINER(button_box), button);

    button = gtk_button_new_with_label("Utilities");
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

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Device",  
            renderer,
            "text", COL_DEV,
            NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Model",  
            renderer,
            "text", COL_MODEL,
            NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeview),
            -1,      
            "Size",  
            renderer,
            "text", COL_SIZE,
            NULL);

    if (!hide_internal) {
        renderer = gtk_cell_renderer_text_new ();
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

