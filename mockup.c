#include<gtk/gtk.h>
#include<stdlib.h>
#include "devinfo.h"
#include "appdefs.h"
#include "dialogs.h"
#include "pages.h"

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

static void activate(GtkApplication* app, gpointer user_data) {
    (void) app;
    (void) user_data;
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *app_box;
    GtkWidget *header_bar;

    app_objects *globals = malloc(sizeof(app_objects));
    ImageInfo *info = malloc(sizeof(ImageInfo));
    globals->user_info = info;

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

    app_box = create_format_selector(globals);
    notebook_append_with_title(globals, app_box, "Target Device Format");

    app_box = create_target_interface(globals);
    notebook_append_with_title(globals, app_box, "Target Image Location Information");

    gtk_widget_show_all(window);

}

static void notebook_append_with_title(app_objects *globals, GtkWidget *ch, const char *title) {
    gtk_notebook_append_page(GTK_NOTEBOOK(globals->notebook), ch, gtk_label_new(title));
}

