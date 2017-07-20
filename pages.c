#include "pages.h"
#include "callbacks.h"
#include "devinfo.h"
#include "dialogs.h"

void set_box_margins(GtkWidget *w) {
    gtk_widget_set_margin_start(w, 30);
    gtk_widget_set_margin_end(w, 30);
    gtk_widget_set_margin_bottom(w, 50);
}

GtkWidget *create_welcome_box(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *button_box;
    GtkWidget *button;
    GtkWidget *evid_device_tv;
    GtkWidget *targ_device_tv;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_EDGE);

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

    globals->etv = evid_device_tv;
    globals->ttv = targ_device_tv;

    button = gtk_button_new_with_label("Utilities");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(check_tv_and_next), globals);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), globals->window);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_container_add(GTK_CONTAINER(app_box), button_box);
    set_box_margins(app_box);

    return app_box;
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
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1,      
            "Device",  
            renderer,
            "text", COL_DEV,
            NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1,      
            "Model",  
            renderer,
            "text", COL_MODEL,
            NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
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

GtkWidget *create_target_interface(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *box;
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *entry;

    globals->os_buttons = malloc(3*sizeof(GtkWidget*));
    
    char *format = "icons/%s_200px.svg";
    char *os_names[] = {"Linux", "Windows", "Apple"};
    char *filename = malloc(strlen(format)+15);

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new(("Please select which types of systems "
                    "you would like to read this drive from "
                    "(Linux/Windows/Apple)")),
            TRUE, TRUE, 0); 

    /* OS Button box */
    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_SPREAD);

    for (int i=0; i<3; i++) {
        sprintf(filename, format, os_names[i]);
        button = gtk_toggle_button_new();
        image = gtk_image_new_from_file(filename);

        gtk_button_set_image(GTK_BUTTON(button), image);
        gtk_widget_set_tooltip_text(button, os_names[i]);

        gtk_container_add(GTK_CONTAINER(box), button);
        globals->os_buttons[i] = button;
    }

    gtk_box_pack_start(GTK_BOX(app_box), box, TRUE, TRUE, 0);

    /* Filename box */
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Filename: "), 
            FALSE, FALSE, 10);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    globals->filename_entry = entry;

    gtk_box_pack_start(GTK_BOX(app_box), box, FALSE, FALSE, 0);

    /* Control button box */
    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_EDGE);
    gtk_widget_set_size_request(box, -1, 100);

    button = gtk_button_new_with_label("Previous");
    gtk_container_add(GTK_CONTAINER(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(notebook_previous_page), globals);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(get_target_info_and_next), globals);
    gtk_container_add(GTK_CONTAINER(box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), globals->window);
    gtk_container_add(GTK_CONTAINER(box), button);

    gtk_box_pack_end(GTK_BOX(app_box), box, FALSE, FALSE, 0);

    free(filename);

    set_box_margins(app_box);

    return app_box;
}
