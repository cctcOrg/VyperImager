#include<stdlib.h>
#include<string.h>
#include "callbacks.h"
#include "appdefs.h"
#include "dialogs.h"
#include "stack.h"
#include "binary_interface.h"

#define MAX_DEV_SIZE 10

enum {
    WELCOME_PAGE = 0,
    EVID_PAGE,
    TGT_PAGE,
    TGTF_PAGE,
    LOC_PAGE,
    CASE_PAGE,
    IMGI_PAGE,
    SUMM_PAGE
};

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
    page_stack *pages = globals->pages; 
    
    int current_page;
    GtkWidget *current_child;
    const char *tab_label;
    int prev_page = pop_stack(pages);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), prev_page);
    current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
    current_child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), current_page);
    tab_label = gtk_notebook_get_tab_label_text(GTK_NOTEBOOK(nb), current_child);
    gtk_header_bar_set_title(GTK_HEADER_BAR(globals->header), tab_label);
}

NEW_CALLBACK(quit_app) {
    (void) w;
    app_objects *globals = udata;
    char *evid_dev;
    char unwriteblock[30];

    if (target_is_mounted())
        system("umount /media/EVID_TARGET");

    evid_dev = globals->user_info->evidence_device;
    if (*evid_dev != NULL) {
        sprintf(unwriteblock, "blockdev --setrw /dev/%s", 
                globals->user_info->evidence_device);
        system(unwriteblock);
    }

    g_application_quit(globals->app);
}

NEW_CALLBACK(welcome_page_cb) {
    (void) w;
    app_objects *globals = udata;

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    push_stack(globals->pages, WELCOME_PAGE);
    set_next_hb_title(globals);
}

NEW_CALLBACK(target_device_cb) {
    (void) w;
    app_objects *globals = udata;
    GtkWidget *window = globals->window;
    ImageInfo *info = globals->user_info;

    GtkTreeSelection *sel;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkWidget *diag;

    char *name;
    GSubprocess *subp;
    char **cmd;

    /* Get target device name */
    sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(globals->ttv));
    if (gtk_tree_selection_get_selected(sel, &model, &iter)) {
        name = malloc(MAX_DEV_SIZE*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->target_device = name;
    } 
    else {
        diag = create_no_device_dialog(window, "target");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(globals->format_dev)))
        gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    /* If you don't need to format skip to the location page */
    else {
        cmd = mount_target_device(info->target_device);
        subp = g_subprocess_newv(cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
        g_subprocess_wait(subp, NULL, NULL);
        g_strfreev(cmd);

        globals->user_info->target_filesystem = "N/A";
        gtk_notebook_set_current_page(GTK_NOTEBOOK(globals->notebook), LOC_PAGE);
    }
    push_stack(globals->pages, TGT_PAGE);
    set_next_hb_title(globals);
}

NEW_CALLBACK(evidence_device_cb) {
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
        name = malloc(MAX_DEV_SIZE*sizeof(char));
        gtk_tree_model_get(model, &iter, COL_DEV, &name, -1);
        info->evidence_device = name;
    } 
    else {
        diag = create_no_device_dialog(window, "evidence");
        gtk_dialog_run(GTK_DIALOG(diag));
        gtk_widget_destroy(diag);
        return;
    }

    writeblock_evidence_device(info->evidence_device);

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    push_stack(globals->pages, EVID_PAGE);
    set_next_hb_title(globals);
}

static void target_setup_done(GtkDialog *d, gint i, gpointer udata) {
    (void) d;
    (void) i;
    app_objects *globals = udata;

    gtk_widget_destroy(globals->dialog);

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));

    push_stack(globals->pages, TGTF_PAGE);
    set_next_hb_title(globals);

}

static void on_mount_finished(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    (void) source_object;
    (void) res;

    app_objects *globals = user_data;
    GtkWidget *diag = globals->dialog;

    gtk_box_pack_start(GTK_BOX(globals->dialog_box), gtk_label_new("Done!"), TRUE, TRUE, 20);
    gtk_spinner_stop(GTK_SPINNER(globals->spinner));
    gtk_widget_show_all(diag);
    g_signal_connect(diag, "response", G_CALLBACK(target_setup_done), globals); 
}

static void on_mkfs_finished(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    (void) source_object;
    (void) res;

    GSubprocess *subp;
    char **cmd;
    app_objects *globals = user_data;

    gtk_box_pack_start(GTK_BOX(globals->dialog_box), 
            gtk_label_new("Mounting..."), TRUE, TRUE, 10);
    gtk_widget_show_all(globals->dialog);

    cmd = mount_target_device(globals->user_info->target_device);
    /* mount_target_device may return NULL, but if it does we still want the 
     * final callback to execute -- so just do nothing */
    if (!cmd) {
        cmd = malloc(2*sizeof(char*));
        cmd[0] = malloc(5*sizeof(char*));
        strcpy(cmd[0], "true"); 
        cmd[1] = NULL;
    }

    subp = g_subprocess_newv(cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
    g_subprocess_wait_async(subp, NULL, on_mount_finished, globals);
    g_strfreev(cmd);
}

NEW_CALLBACK(format_device_cb) {
    (void) w;

    GtkWidget *diag;

    app_objects *globals = udata;
    GtkWidget *window = globals->window;
    ImageInfo *info = globals->user_info;

    char buttons_state = 0;
    char *fs_choice = malloc(7*sizeof(char));
    int result;

    GSubprocess *subp;
    char **cmd;

    for (int i=0; i<3; i++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(globals->os_buttons[i])))
            buttons_state |= 1<<i;
    }
    
    switch (buttons_state) {
        /* Windows, Mac, Linux */
        case 0b011:
        case 0b111:
        case 0b110:
        case 0b010:
            strcpy(fs_choice, "ntfs");
            break;
        /* Mac, Linux */
        case 0b100:
        case 0b101:
            strcpy(fs_choice, "hfsplus");
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

    diag = create_confirm_target_device_dialog(window, info->target_device);
    result = gtk_dialog_run(GTK_DIALOG(diag));
    gtk_widget_destroy(diag);
    if (result != GTK_RESPONSE_ACCEPT)
        return;
    
    diag = create_progress_spinner_dialog(window, globals); 
    globals->dialog = diag;
    
    gtk_box_pack_start(GTK_BOX(globals->dialog_box), gtk_label_new("Formatting..."), TRUE, TRUE, 10);
    gtk_widget_show_all(diag);

    cmd = format_target_device(info->target_device, fs_choice);
    subp = g_subprocess_newv(cmd, G_SUBPROCESS_FLAGS_NONE, NULL);
    g_subprocess_wait_async(subp, NULL, on_mkfs_finished, globals);
    g_strfreev(cmd);
}

NEW_CALLBACK(get_target_info_cb) {
    (void) w;
    app_objects *globals = udata;

    /*GtkWidget *diag;*/
    ImageInfo *info = globals->user_info;
    GtkWidget *entry;

    char *entry_data;
    const char *entry_tmp;
    
    entry = globals->filename_entry;

    entry_tmp = gtk_entry_get_text(GTK_ENTRY(globals->filename_entry));
    entry_data = malloc((strlen(entry_tmp)+1)*sizeof(char));
    strcpy(entry_data, entry_tmp);
    info->target_filename = entry_data;

    entry = globals->directory_entry;
    entry_tmp = g_file_get_path(
            gtk_file_chooser_get_current_folder_file(GTK_FILE_CHOOSER(entry)));
    entry_data = malloc((strlen(entry_tmp)+1)*sizeof(char));
    strcpy(entry_data, entry_tmp);
    info->target_directory = entry_data;

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    push_stack(globals->pages, LOC_PAGE);
    set_next_hb_title(globals);
}

NEW_CALLBACK(case_info_cb) {
    (void) w;
    app_objects *g = udata;
    Case *c = g->user_info->case_info;
    
    GtkTextIter start, end;
    GtkWidget *entry;
    const char *entry_tmp;
    char *entry_data;

    GtkWidget *entries[] = {g->casenum_entry, g->itemnum_entry, 
        g->examiner_entry, g->notes_entry};

    char **info[] = {&c->casenum, &c->itemnum, &c->examiner, &c->notes};

    for (int i=0; i<4; i++) {
        entry = entries[i];

        entry_tmp = gtk_entry_get_text(GTK_ENTRY(entry));
        entry_data = malloc((strlen(entry_tmp)+1)*sizeof(char));
        /*printf("%p\n", entry_data);*/
        strcpy(entry_data, entry_tmp);
        *info[i] = entry_data;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g->desc_entry));

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    entry_tmp = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    entry_data = malloc((strlen(entry_tmp)+1)*sizeof(char));
    strcpy(entry_data, entry_tmp);
    c->desc = entry_data;
    
    gtk_notebook_next_page(GTK_NOTEBOOK(g->notebook));
    push_stack(g->pages, CASE_PAGE);
    set_next_hb_title(g);
}

NEW_CALLBACK(image_info_cb) {
    (void) w;
    char str[50];

    app_objects *globals = udata;
    GtkWidget *label;
    ImageInfo *info = globals->user_info;
    SummaryLabels *labels = globals->labels;
    Case *case_info = info->case_info;

    globals->user_info->device_type = gtk_combo_box_text_get_active_text(
            GTK_COMBO_BOX_TEXT(globals->devtype_combobox));

    globals->user_info->hash_type = gtk_combo_box_text_get_active_text(
            GTK_COMBO_BOX_TEXT(globals->hashtype_combobox));

    globals->user_info->compression_type = gtk_combo_box_text_get_active_text(
            GTK_COMBO_BOX_TEXT(globals->comptype_combobox));


    /* Device info */
    sprintf(str, "/dev/%s", info->evidence_device); 
    label = labels->evidence_device;
    gtk_label_set_text(GTK_LABEL(label), str);

    sprintf(str, "/dev/%s", info->target_device); 
    label = labels->target_device;
    gtk_label_set_text(GTK_LABEL(label), str);

    label = labels->target_filesystem;
    gtk_label_set_text(GTK_LABEL(label), info->target_filesystem);

    label = labels->target_filename;
    gtk_label_set_text(GTK_LABEL(label), info->target_filename);
    
    label = labels->target_directory;
    gtk_label_set_text(GTK_LABEL(label), info->target_directory);

    /* Case info */
    label = labels->casenum;
    gtk_label_set_text(GTK_LABEL(label), case_info->casenum);

    label = labels->itemnum;
    gtk_label_set_text(GTK_LABEL(label), case_info->itemnum);

    label = labels->examiner;
    gtk_label_set_text(GTK_LABEL(label), case_info->examiner);

    label = labels->desc;
    gtk_label_set_text(GTK_LABEL(label), case_info->desc);

    label = labels->notes;
    gtk_label_set_text(GTK_LABEL(label), case_info->notes);

    /* Image metadata */
    label = labels->device_type;
    gtk_label_set_text(GTK_LABEL(label), info->device_type);

    label = labels->compression_type;
    gtk_label_set_text(GTK_LABEL(label), info->compression_type);

    label = labels->hash_type;
    gtk_label_set_text(GTK_LABEL(label), info->hash_type);

    gtk_notebook_next_page(GTK_NOTEBOOK(globals->notebook));
    push_stack(globals->pages, IMGI_PAGE);
    set_next_hb_title(globals);
}
