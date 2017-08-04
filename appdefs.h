#ifndef APPDEFS_H
#define APPDEFS_H

#include "stack.h"

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
    char *target_filesystem;
    char *target_filename;
    char *target_directory;
    char *device_type;
    char *hash_type;
    char *compression_type;
    Case *case_info;
} ImageInfo;

enum {
    COL_DEV = 0,
    COL_MODEL,
    COL_SIZE,
    COL_REMOVABLE,
    COL_ISTARGET,
    NUM_COLS
};

typedef struct app_objects {
    page_stack *pages;

    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *header;

    GtkWidget *etv;
    GtkWidget *ttv;
    GtkWidget *format_dev;

    GtkWidget *os_button_box;
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
    
    ImageInfo *user_info;
} app_objects;

#endif /* APPDEFS_H */
