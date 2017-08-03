#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>

#include "pages.h"
#include "callbacks.h"
#include "devinfo.h"
#include "dialogs.h"

static void set_box_margins(GtkWidget *w) {
    gtk_widget_set_margin_start(w, 30);
    gtk_widget_set_margin_end(w, 30);
    gtk_widget_set_margin_bottom(w, 50);
    gtk_widget_set_margin_top(w, 20);
}

static void create_navigation_button_box(GtkWidget *ab, 
        void (*cb)(GtkWidget *, gpointer), app_objects *globals) {

    GtkWidget *box;
    GtkWidget *button;

    /* Control button box */
    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_EDGE);
    gtk_widget_set_size_request(box, -1, 100);

    button = gtk_button_new_with_label("Previous");
    gtk_container_add(GTK_CONTAINER(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(notebook_previous_page), globals);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(cb), globals);
    gtk_container_add(GTK_CONTAINER(box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), globals->window);
    gtk_container_add(GTK_CONTAINER(box), button);

    gtk_box_pack_end(GTK_BOX(ab), box, FALSE, FALSE, 0);
}

static GtkTreeModel *create_block_devices_liststore(int hide_internal, GtkTreePath **path) {
    GtkListStore  *store;
    GtkTreeIter    iter;

    int num_blockdevs;
    Device **blockdev_info;
    Device *dev;

    /* Device, model, size */
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, 
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    blockdev_info = get_blockdev_info(&num_blockdevs);

    for (int i=0; i<num_blockdevs; i++) {
        dev = blockdev_info[i];

        if (hide_internal)
            if (!dev->removable)
                continue;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                COL_DEV, dev->name, COL_MODEL, dev->model, COL_SIZE, dev->size,
                COL_REMOVABLE, (dev->removable) ? "Yes" : "No",
                COL_ISTARGET, dev->is_target,
                -1);
        if (dev->is_target)
            *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    }
    return GTK_TREE_MODEL(store);
} 

static GtkWidget *create_block_devices_treeview(int hide_internal, app_objects *globals) {
    GtkTreeSelection    *selection;
    GtkCellRenderer     *renderer;
    GtkTreeModel        *model;
    GtkTreePath         *evid_path = NULL;
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
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
                -1,      
                "Removable",  
                renderer,
                "text", COL_REMOVABLE,
                NULL);
    }

    model = create_block_devices_liststore(hide_internal, &evid_path);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);

    if (evid_path && hide_internal) {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
        gtk_tree_selection_select_path(selection, evid_path);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(globals->format_dev), FALSE);
    }

    g_object_unref(model);

    return treeview;
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

    gtk_widget_set_size_request(button_box, -1, 100);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Welcome to the CCTC Imaging Toolkit!"),
                TRUE, TRUE, 30);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the device to image"),
                TRUE, TRUE, 10);

    /* This needs to be defined before the treeviews because the treeviews
     * depend on this button because they'll activate it if EVID_TARGET
     * is not found 
     */
    button = gtk_check_button_new_with_label("Format?");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    globals->format_dev = button;

    /* Treeviews */
    evid_device_tv = create_block_devices_treeview(SHOW_INT, globals);
    gtk_box_pack_start(GTK_BOX(app_box), evid_device_tv, TRUE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the target device"),
                TRUE, TRUE, 20);

    targ_device_tv = create_block_devices_treeview(HIDE_INT, globals);
    gtk_box_pack_start(GTK_BOX(app_box), targ_device_tv, TRUE, TRUE, 10);

    globals->etv = evid_device_tv;
    globals->ttv = targ_device_tv;

    /* Format button */
    gtk_box_pack_start(GTK_BOX(app_box), button, TRUE, TRUE, 20);

    /* Button box */
    button = gtk_button_new_with_label("Utilities");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(check_tv_cb), globals);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), globals->window);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_box_pack_end(GTK_BOX(app_box), button_box, FALSE, FALSE, 0);
    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_format_selector(app_objects *globals) {
    GtkWidget *app_box, *box, *obox, *button, *image;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    globals->os_buttons = malloc(3*sizeof(GtkWidget*));
    
    char *format = "icons/%s_200px.svg";
    char *os_names[] = {"Linux", "Windows", "Apple"};
    char *filename = malloc(strlen(format)+15);

    obox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(obox), 
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

    gtk_box_pack_start(GTK_BOX(obox), box, TRUE, TRUE, 0);
    globals->os_button_box = obox;
    gtk_box_pack_start(GTK_BOX(app_box), obox, FALSE, FALSE, 0);
    
    create_navigation_button_box(app_box, format_device_cb, globals);
    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_target_interface(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *grid;
    GtkWidget *button;
    GtkWidget *entry;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    /* Filename widgets */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Filename (no extension): "),
            0, 0, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);
    globals->filename_entry = entry;

    /* Directory box */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Directory: "), 0, 1, 1, 1);

    button = gtk_file_chooser_button_new("Select a folder to save the the image in",
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    globals->directory_entry = button;
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(button), "/media/EVID_TARGET");
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);


    create_navigation_button_box(app_box, get_target_info_cb, globals);

    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_case_metadata_interface(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *grid;
    /*GtkWidget *button;*/
    GtkWidget *entry;
    GtkWidget *frame;
    GtkWidget *scrolled_window;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(grid, 600, 600);

    /* Case Number */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Case number: "),
            0, 0, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);

    /* Item Number */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Item number: "),
            0, 1, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);

    /* Examiner */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Examiner: "),
            0, 2, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 2, 1, 1);

    /* Description */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Description: "),
            0, 3, 1, 1);

    frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(frame, 200, 200);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 3);

    entry = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled_window), entry);
    gtk_container_add(GTK_CONTAINER(frame), scrolled_window);

    gtk_grid_attach(GTK_GRID(grid), frame, 1, 3, 1, 1);

    /* Notes */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Notes: "),
            0, 4, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 4, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);
    create_navigation_button_box(app_box, get_target_info_cb, globals);

    set_box_margins(app_box);

    return app_box;
}
