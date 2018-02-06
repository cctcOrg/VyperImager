#ifndef APPDEFS_H
#define APPDEFS_H

#include<gtk/gtk.h>
#include "stack.h"

/* This is for the GtkTreeModel's internal store */
enum {
    COL_DEV = 0,
    COL_MODEL,
    COL_SIZE,
    COL_NPARTS,
    COL_LABELS,
    COL_REMOVABLE,
    COL_ISTARGET,
    COL_PATH,
    NUM_COLS
};

typedef struct SummaryLabels {
    GtkWidget *evidence_device;
    GtkWidget *target_device;
    GtkWidget *target_filesystem;
    GtkWidget *target_filename;
    GtkWidget *target_directory;
    GtkWidget *device_type;
    GtkWidget *hash_type;
    GtkWidget *compression_type;
    GtkWidget *casenum;
    GtkWidget *itemnum;
    GtkWidget *examiner;
    GtkWidget *desc;
    GtkWidget *notes;
} SummaryLabels;

typedef struct Case {
    char *casenum;
    char *itemnum;
    char *examiner;
    char *desc;
    char *notes;
} Case;

typedef struct ImageInfo {
    char *evidence_device;
    char *target_device;
    char *evd_path;
    char *tgt_path;
    char *target_filesystem;
    char *target_filename;
    char *target_directory;
    char *device_type;
    char *hash_type;
    char *compression_type;
    Case *case_info;
} ImageInfo;

typedef struct app_objects {
    page_stack *pages;

    GtkApplication *app;

    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *header;

    GtkWidget *etv;
    GtkWidget *ttv;
    GtkWidget *format_dev;

    GtkWidget **os_buttons;

    GtkWidget *filename_entry;
    GtkWidget *directory_entry;

    GtkWidget *casenum_entry;
    GtkWidget *itemnum_entry;
    GtkWidget *examiner_entry;
    GtkWidget *desc_entry;
    GtkWidget *notes_entry;

    GtkWidget *devtype_combobox;
    GtkWidget *hashtype_combobox;
    GtkWidget *comptype_combobox;

    GtkWidget *dialog;
    GtkWidget *dialog_box;
    GtkWidget *dialog_button;
    GtkWidget *spinner;
    GtkWidget *prog_bar;
    
    ImageInfo *user_info;
    SummaryLabels *labels;
} app_objects;

// Implemented in main.c
void print_globals(app_objects *a);

#endif /* APPDEFS_H */
