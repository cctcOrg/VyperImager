#include<gtk/gtk.h>
#include "devinfo.h"
#include "image_types.h"
#include "dialogs.h"

typedef struct cb_data {
    GtkWidget *ttv;
    GtkWidget *etv;
    GtkWidget **buttons;
    GtkWidget *entry;
} cb_data; 

typedef struct globals {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *header;
    ImageInfo *user_info;
} globals;

static void activate(GtkApplication *app, gpointer user_data);
void set_box_margins(GtkWidget *w);
void notebook_append_with_title(GtkWidget *nb, const char *title);
void notebook_previous_page();
GtkWidget *create_welcome_box(GtkWidget *window);
GtkWidget *create_block_devices_treeview();
GtkWidget *create_target_interface(GtkWidget *window);

// Callbacks
void check_tv_and_next(GtkWidget *w, gpointer udata);
void get_target_info_and_next(GtkWidget *w, gpointer udata);

globals g_OBJS;  

int main (int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *app_box;
    GtkWidget *header_bar;

    ImageInfo *info = malloc(sizeof(ImageInfo*));
    g_OBJS.user_info = info;

    window = gtk_application_window_new(app);
    g_OBJS.window = window;

    header_bar = gtk_header_bar_new();
    g_OBJS.header = header_bar;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(app_box), header_bar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), app_box);

    notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);

    g_OBJS.notebook = notebook;
    gtk_box_pack_end(GTK_BOX(app_box), notebook, TRUE, TRUE, 0);

    gtk_window_set_title(GTK_WINDOW(window), "CCTC Imaging ToolKit");
    gtk_window_fullscreen(GTK_WINDOW(window));

    app_box = create_welcome_box(window);
    notebook_append_with_title(app_box, "Evidence and Target Devices");

    app_box = create_target_interface(window);
    notebook_append_with_title(app_box, "Target Device Information");

    gtk_widget_show_all(window);
}

void set_box_margins(GtkWidget *w) {
    gtk_widget_set_margin_start(w, 30);
    gtk_widget_set_margin_end(w, 30);
    gtk_widget_set_margin_bottom(w, 50);
}

void notebook_append_with_title(GtkWidget *ch, const char *title) {
    gtk_notebook_append_page(GTK_NOTEBOOK(g_OBJS.notebook), ch, gtk_label_new(title));
    gtk_header_bar_set_title(GTK_HEADER_BAR(g_OBJS.header), title);
}

void notebook_previous_page(GtkWidget *button, gpointer data) {
    GtkWidget *nb = g_OBJS.notebook;
    
    int current_page;
    GtkWidget *current_child;
    const char *tab_label;
    
    gtk_notebook_prev_page(GTK_NOTEBOOK(nb));
    current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
    current_child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), current_page);
    tab_label = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(nb), current_child);
    gtk_header_bar_set_title(GTK_HEADER_BAR(g_OBJS.header), tab_label);
}

GtkWidget *create_welcome_box(GtkWidget *window) {
    GtkWidget *app_box;
    GtkWidget *button_box;
    GtkWidget *button;
    GtkWidget *evid_device_tv;
    GtkWidget *targ_device_tv;

    cb_data *udata = malloc(sizeof(cb_data*));

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

    udata->etv = evid_device_tv;
    udata->ttv = targ_device_tv;

    button = gtk_button_new_with_label("Utilities");
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(check_tv_and_next), udata);
    gtk_container_add(GTK_CONTAINER(button_box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), window);
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
    cb_data *data = udata;
    ImageInfo *info = g_OBJS.user_info;

    /* Get evidence device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(data->etv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(4*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->evidence_device = name;
    } 
    else {
        diag = create_no_device_dialog(g_OBJS.window, "evidence");
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
        diag = create_no_device_dialog(g_OBJS.window, "target");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }
    
    if (strcmp(info->evidence_device, info->target_device) == 0) {
        diag = create_same_device_dialog(g_OBJS.window);
        gtk_dialog_run(GTK_DIALOG(diag));
        return;
    }

    diag = create_confirm_target_device_dialog(g_OBJS.window, info->target_device);
    int result = gtk_dialog_run(GTK_DIALOG(diag));
    gtk_widget_destroy(diag);

    if (result != GTK_RESPONSE_ACCEPT)
        return;

    gtk_notebook_next_page(GTK_NOTEBOOK(g_OBJS.notebook));
}

GtkWidget *create_target_interface(GtkWidget *window) {
    GtkWidget *app_box;
    GtkWidget *box;
    GtkWidget *button;
    GtkWidget *image;
    GtkWidget *entry;

    cb_data *udata = malloc(sizeof(cb_data*));
    udata->buttons = malloc(3*sizeof(GtkWidget*));
    
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
        udata->buttons[i] = button;
    }

    gtk_box_pack_start(GTK_BOX(app_box), box, TRUE, TRUE, 0);

    /* Filename box */
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(box), gtk_label_new("Filename: "), 
            FALSE, FALSE, 10);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    udata->entry = entry;

    gtk_box_pack_start(GTK_BOX(app_box), box, FALSE, FALSE, 0);

    /* Control button box */
    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_EDGE);
    gtk_widget_set_size_request(box, -1, 100);

    button = gtk_button_new_with_label("Previous");
    gtk_container_add(GTK_CONTAINER(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(notebook_previous_page), NULL);
    
    button = gtk_button_new_with_label("Next");
    gtk_widget_set_size_request(button, -1, 50);
    g_signal_connect(button, "clicked", G_CALLBACK(get_target_info_and_next), udata);
    gtk_container_add(GTK_CONTAINER(box), button);
    
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", 
            G_CALLBACK(gtk_widget_destroy), window);
    gtk_container_add(GTK_CONTAINER(box), button);

    gtk_box_pack_end(GTK_BOX(app_box), box, FALSE, FALSE, 0);

    free(filename);

    set_box_margins(app_box);

    return app_box;
}

void get_target_info_and_next(GtkWidget *w, gpointer udata) {
    GtkWidget *diag;
    GtkWidget *entry;

    cb_data *data = udata;
    char buttons_state = 0;
    char *fs_choice = malloc(7*sizeof(char));
    char *entry_data;
    char *entry_tmp;
    
    entry = data->entry;

    for (int i=0; i<3; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data->buttons[i])))
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
            diag = create_please_choose_system_dialog(g_OBJS.window);
            gtk_dialog_run(GTK_DIALOG(diag));
            gtk_widget_destroy(diag);
            return;
    }

    entry_tmp = gtk_entry_get_text(GTK_ENTRY(entry));
    entry_data = malloc(strlen(entry_tmp)*sizeof(char));
    strcpy(entry_data, entry_tmp);
    printf("%s\n", entry_data);

    g_OBJS.user_info->target_filesystem = fs_choice; 

    free(data->buttons);
    free(udata);

    /*gtk_notebook_next_page(GTK_NOTEBOOK(g_OBJS.notebook));*/
}
