#include<gtk/gtk.h>
#include<stdlib.h>
#include "devinfo.h"
#include "appdefs.h"
#include "dialogs.h"
#include "callbacks.h"
#include "pages.h"
#include "stack.h"

static void activate(GtkApplication *app, gpointer user_data);
static void notebook_append_with_title(app_objects *globals, GtkWidget *nb, const char *title);

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
    (void) user_data;

    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *app_box;
    GtkWidget *header_bar;

    app_objects *globals = malloc(sizeof(app_objects));
    globals->app = app;
    
    ImageInfo *info = malloc(sizeof(ImageInfo));
    globals->user_info = info;

    Case *case_info = malloc(sizeof(Case));
    globals->user_info->case_info = case_info;

    SummaryLabels *labels = malloc(sizeof(SummaryLabels));
    globals->labels = labels;

    page_stack *p = new_stack();
    globals->pages = p;

    window = gtk_application_window_new(app);
    globals->window = window;

    header_bar = gtk_header_bar_new();
    globals->header = header_bar;

    app_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(app_box), header_bar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), app_box);

    notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);

    globals->notebook = notebook;
    gtk_box_pack_end(GTK_BOX(app_box), notebook, TRUE, TRUE, 0);

    gtk_window_set_title(GTK_WINDOW(window), "CCTC Imaging ToolKit");
    gtk_window_fullscreen(GTK_WINDOW(window));

    char *title = "Evidence and Target Devices";
    app_box = create_welcome_box(globals);
    /* globals contains the notebook and header bar */
    notebook_append_with_title(globals, app_box, title);
    /* Set header bar title to the first page's title
     * (callbacks subsequently handle this) */
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), title);

    app_box = create_evidence_selector(globals);
    notebook_append_with_title(globals, app_box, "Evidence Device Selection");

    app_box = create_target_selector(globals);
    notebook_append_with_title(globals, app_box, "Target Device Selection");

    app_box = create_format_selector(globals);
    notebook_append_with_title(globals, app_box, "Target Device Format");

    app_box = create_target_interface(globals);
    notebook_append_with_title(globals, app_box, "Target Image Location Information");

    app_box = create_case_metadata_interface(globals);
    notebook_append_with_title(globals, app_box, "Case Metadata");

    app_box = create_image_metadata_interface(globals);
    notebook_append_with_title(globals, app_box, "Image Metadata");

    app_box = create_summary_interface(globals);
    notebook_append_with_title(globals, app_box, "Summary");

    g_signal_connect(window, "delete-event", G_CALLBACK(quit_app), globals);
    gtk_widget_show_all(window);

}

static void notebook_append_with_title(app_objects *globals, GtkWidget *ch, const char *title) {
    gtk_notebook_append_page(GTK_NOTEBOOK(globals->notebook), ch, gtk_label_new(title));
}

// Defined in appdefs.h, somewhat arbitrarily implemented here
void print_globals(app_objects *a) {
    g_print("\33[31mMain objects:\33[0m\n");
    g_print("App: %p\n", a->app);

    g_print("Window: %p\n", a->window);
    g_print("Notebook: %p\n", a->notebook);
    g_print("Header: %p\n", a->header);

    g_print("%p\n", a->etv);
    g_print("%p\n", a->ttv);
    g_print("%p\n", a->format_dev);

    g_print("%p\n", a->os_buttons);

    g_print("%p\n", a->filename_entry);
    g_print("%p\n", a->directory_entry);

    g_print("%p", a->casenum_entry);
    g_print("%p", a->itemnum_entry);
    g_print("%p", a->examiner_entry);
    g_print("%p", a->desc_entry);
    g_print("%p", a->notes_entry);

    g_print("%p", a->devtype_combobox);
    g_print("%p", a->hashtype_combobox);
    g_print("%p", a->comptype_combobox);

    g_print("\nDialog: %p\n", a->dialog);
    g_print("Dialog box: %p\n", a->dialog_box);
    g_print("Dialog button: %p\n", a->dialog_button);
    g_print("Spinner: %p\n", a->spinner);
    g_print("Progress bar: %p\n", a->prog_bar);

    g_print("\n\33[31mImage Info :\33[0m\n");

    g_print("%s\n", a->user_info->evidence_device);
    g_print("%s\n", a->user_info->target_device);
    g_print("%s\n", a->user_info->evd_path);
    g_print("%s\n", a->user_info->tgt_path);
    g_print("%s\n", a->user_info->target_filesystem);
    g_print("%s\n", a->user_info->target_filename);
    g_print("%s\n", a->user_info->target_directory);
    g_print("%s\n", a->user_info->device_type);
    g_print("%s\n", a->user_info->hash_type);
    g_print("%s\n", a->user_info->compression_type);

    g_print("\n\33[31mCase Info :\33[0m\n");

    g_print("%s\n", a->user_info->case_info->casenum);
    g_print("%s\n", a->user_info->case_info->itemnum);
    g_print("%s\n", a->user_info->case_info->examiner);
    g_print("%s\n", a->user_info->case_info->desc);
    g_print("%s\n", a->user_info->case_info->notes);
}
