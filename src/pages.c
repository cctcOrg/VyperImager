#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>

#include "pages.h"
#include "callbacks.h"
#include "devinfo.h"
#include "dialogs.h"

#define GRID_LEFT_MARGIN 200

static void set_box_margins(GtkWidget *w) {
    gtk_widget_set_margin_start(w, 30);
    gtk_widget_set_margin_end(w, 30);
    gtk_widget_set_margin_bottom(w, 50);
    gtk_widget_set_margin_top(w, 20);
}

/* Used to create the Previous/Next/Quit button box. It will automatically pack
 * the created box into the passed ab (app box) parameter */
static void create_navigation_button_box(GtkWidget *ab, 
        void (*cb)(GtkWidget *, gpointer), app_objects *globals)
{
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
    g_signal_connect(button, "clicked", G_CALLBACK(quit_app), globals);
    gtk_container_add(GTK_CONTAINER(box), button);

    gtk_box_pack_end(GTK_BOX(ab), box, FALSE, FALSE, 0);
 }

static GtkTreeModel *create_block_devices_liststore(
        int hide_internal, GtkTreePath **path, char *evid)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    int num_blockdevs;
    int is_eviddev;
    
    int is_first = 1;
    char *label;
    char *partlabels;

    Device **blockdev_info;
    Device *dev;

    /* Device, model, size */
    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, 
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING,
            G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
    blockdev_info = get_blockdev_info(&num_blockdevs);

    for (int i=0; i<num_blockdevs; i++) {
        partlabels = "";
        dev = blockdev_info[i];
 
        is_eviddev = ( evid != NULL  &&  (strcmp(evid, dev->name) == 0) ); 
        
        for (int j = 0; j < dev->numparts; j++) {
            label = dev->labels[j];
            if (label != NULL) {
                if (is_first) {
                    partlabels = malloc(strlen(label)+1);
                    strcpy(partlabels, label);
                    is_first = 0;
                } 
                else {
                    /* Need to allocate 2 extra spaces for the ", " part and
                     * and extra byte for the terminating null byte */
                    partlabels = realloc(partlabels, 
                            (strlen(partlabels) + strlen(label) + 3));
                    strcat(partlabels, ", ");
                    strcat(partlabels, label);
                }
            }
        }
        /* Forget to reset this */
        is_first = 1;

        /* If we've been asked to hide the internal devices, then this is the
         * target selection interface. In that case we should skip any device
         * that isn't removable, but also we want to skip the evidence device */
        if (hide_internal && (!dev->removable || is_eviddev))
            continue;

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                COL_DEV, dev->name, COL_MODEL, dev->model, COL_SIZE, dev->size,
                COL_NPARTS, dev->numparts, COL_LABELS, partlabels,
                COL_REMOVABLE, (dev->removable) ? "Yes" : "No",
                COL_ISTARGET, dev->is_target, COL_PATH, dev->fspath, -1);

        if (dev->is_target)
            *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    }
    return GTK_TREE_MODEL(store);
} 

static GtkWidget *create_block_devices_treeview(int hide_internal) {
    GtkTreeSelection    *selection;
    GtkCellRenderer     *renderer;
    /*GtkTreeModel        *model;*/
    /*GtkTreePath         *evid_path = NULL;*/
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

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1,      
            "Number of Partitions",  
            renderer,
            "text", COL_NPARTS,
            NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1,      
            "Partition Labels",  
            renderer,
            "text", COL_LABELS,
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

    return treeview;
}

void set_treeview_model(GtkWidget *treeview, int hide_internal, app_objects *globals) {
    GtkTreeSelection    *selection;
    GtkTreeModel        *model;
    GtkTreePath         *evid_path = NULL;
    char                *e = globals->user_info->evidence_device;

    model = create_block_devices_liststore(hide_internal, &evid_path, e);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);

    if (evid_path && hide_internal) {
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
        gtk_tree_selection_select_path(selection, evid_path);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(globals->format_dev), FALSE);
    }

    g_object_unref(model);
}

GtkWidget *create_welcome_box(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *button_box;
    GtkWidget *button;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_EDGE);

    gtk_widget_set_size_request(button_box, -1, 100);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Welcome to the First Responder Imaging Toolkit!"),
                TRUE, TRUE, 30);

    /* Button box */
    button = gtk_button_new_with_label("Utilities");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(welcome_page_cb), globals);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect(button, "clicked", G_CALLBACK(quit_app), globals);
    gtk_container_add(GTK_CONTAINER(button_box), button);

    gtk_box_pack_end(GTK_BOX(app_box), button_box, FALSE, FALSE, 0);
    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_target_selector(app_objects *globals) {
    GtkWidget *app_box;
    GtkWidget *button;
    GtkWidget *targ_device_tv;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose which device you'd like to save your "
                "image on (i.e the Target Device)"),
                TRUE, TRUE, 10);

    /* This needs to be defined before the treeviews because the treeviews
     * depend on this button because they'll activate it if EVID_TARGET
     * is not found 
     */
    button = gtk_check_button_new_with_label("Format?");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    globals->format_dev = button;

    targ_device_tv = create_block_devices_treeview(HIDE_INT);
    gtk_box_pack_start(GTK_BOX(app_box), targ_device_tv, TRUE, TRUE, 10);
    globals->ttv = targ_device_tv;

    /* Format button */
    gtk_box_pack_start(GTK_BOX(app_box), button, TRUE, TRUE, 20);

    create_navigation_button_box(app_box, target_device_cb, globals);
    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_evidence_selector(app_objects *globals) {
    GtkWidget *app_box;
    /*GtkWidget *button;*/
    GtkWidget *evid_device_tv;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_box_pack_start(GTK_BOX(app_box), 
            gtk_label_new("Please choose the evidence device"), TRUE, TRUE, 10);

    /* Treeviews */
    evid_device_tv = create_block_devices_treeview(SHOW_INT);
    gtk_box_pack_start(GTK_BOX(app_box), evid_device_tv, TRUE, TRUE, 10);

    globals->etv = evid_device_tv;

    create_navigation_button_box(app_box, evidence_device_cb, globals);
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
    gtk_widget_set_valign(obox, GTK_ALIGN_CENTER);

    gtk_box_pack_start(GTK_BOX(obox), 
            gtk_label_new(("Please select which types of systems "
                    "you would like to read this drive from "
                    "(Linux/Windows/Apple)")),
            TRUE, TRUE, 20); 

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
    gtk_box_pack_start(GTK_BOX(app_box), obox, TRUE, TRUE, 0);
    
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
    globals->casenum_entry = entry;

    /* Item Number */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Item number: "),
            0, 1, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    globals->itemnum_entry = entry;

    /* Examiner */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Examiner: "),
            0, 2, 1, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 2, 1, 1);
    globals->examiner_entry = entry;

    /* Description */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Description: "),
            0, 3, 1, 1);

    frame = gtk_frame_new(NULL);
    gtk_widget_set_size_request(frame, 200, 200);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 3);

    entry = gtk_text_view_new();
    globals->desc_entry = entry;
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(entry), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled_window), entry);
    gtk_container_add(GTK_CONTAINER(frame), scrolled_window);

    gtk_grid_attach(GTK_GRID(grid), frame, 1, 3, 1, 1);

    /* Notes */
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Notes: "),
            0, 4, 1, 1);

    entry = gtk_entry_new();
    globals->notes_entry = entry;
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 4, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);
    create_navigation_button_box(app_box, case_info_cb, globals);

    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_image_metadata_interface(app_objects *globals) {
    GtkWidget *combobox;
    GtkWidget *label;
    GtkWidget *app_box;
    GtkWidget *grid;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    /*gtk_widget_set_size_request(grid, 600, 600);*/


    const char *overview = "Please select from one of the following:\n" 
            "- Fixed: internal drives\n"
            "- Removable: Flash drives/SD cards/External hard drives\n"
            "- Optical: CD/DVD, any kind of disc\n"
            "- Memory: RAM";
    label = gtk_label_new(overview);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Fixed");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Removable");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Optical");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Memory");

    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
    gtk_grid_attach(GTK_GRID(grid), combobox, 1, 0, 1, 1);
    globals->devtype_combobox = combobox; 
    
    const char * verification= "Please select a verification hash:\n"
        "- SHA1: Fastest, but will not give complete confidence \n"
        "- SHA256: Use this for a real case";
    label = gtk_label_new(verification);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "SHA1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "SHA256");

    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
    gtk_grid_attach(GTK_GRID(grid), combobox, 1, 1, 1, 1);
    globals->hashtype_combobox = combobox; 


    const char *compression = "Please select a compression type:\n"
        "- None: Fastest, image will be the size of the drive\n"
        "- Fast: Middle ground for speed and size\n"
        "- Best: Will make image as small as possible, "
            "may take significantly longer on older hardware"; 
    label = gtk_label_new(compression);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "None");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Best");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), "Fast");

    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
    gtk_grid_attach(GTK_GRID(grid), combobox, 1, 2, 1, 1);
    globals->comptype_combobox = combobox; 


    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);
    create_navigation_button_box(app_box, image_info_cb, globals);

    set_box_margins(app_box);

    return app_box;
}

GtkWidget *create_summary_interface(app_objects *globals) {
    GtkWidget *label;
    GtkWidget *app_box;
    GtkWidget *grid;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    /* Target grid */
    grid = gtk_grid_new();
    gtk_widget_set_margin_start(grid, GRID_LEFT_MARGIN);
    gtk_widget_set_margin_end(grid, GRID_LEFT_MARGIN);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    /* Evidence Device */
    label = gtk_label_new("Evidence device:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->evidence_device = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    /* Target Device */
    label = gtk_label_new("Target device:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->target_device = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);
    
    /* Target Filesystem */
    label = gtk_label_new("Target filesystem:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->target_filesystem = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 2, 1, 1);

    /* Target Directory */
    label = gtk_label_new("Target directory:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->target_directory = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 3, 1, 1);

    /* Target Filename */
    label = gtk_label_new("Target filename:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->target_filename = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 4, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);

    /* Case grid */
    grid = gtk_grid_new();
    gtk_widget_set_margin_start(grid, GRID_LEFT_MARGIN);
    gtk_widget_set_margin_end(grid, GRID_LEFT_MARGIN);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    label = gtk_label_new("Case number:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->casenum = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    label = gtk_label_new("Item number:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->itemnum = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);

    label = gtk_label_new("Examiner:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->examiner = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 2, 1, 1);

    label = gtk_label_new("Description:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->desc = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 3, 1, 1);

    label = gtk_label_new("Notes:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->notes = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 4, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);

    /* Image grid */
    grid = gtk_grid_new();
    gtk_widget_set_margin_start(grid, GRID_LEFT_MARGIN);
    gtk_widget_set_margin_end(grid, GRID_LEFT_MARGIN);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    label = gtk_label_new("Device type:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->device_type = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    label = gtk_label_new("Hash type:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->hash_type = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);

    label = gtk_label_new("Compression type:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    label = gtk_label_new(NULL);
    globals->labels->compression_type = label;
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 2, 1, 1);

    gtk_box_pack_start(GTK_BOX(app_box), grid, TRUE, TRUE, 0);

    create_navigation_button_box(app_box, create_image_cb, globals);
    set_box_margins(app_box);

    return app_box;
}
