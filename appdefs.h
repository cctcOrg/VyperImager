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
    int device_type;
    int hash_type;
    int compression_type;
    Case *case_info;
} ImageInfo;

enum {
    COL_DEV = 0,
    COL_MODEL,
    COL_SIZE,
    COL_REMOVABLE,
    NUM_COLS
};

//typedef struct cb_data {
    //GtkWidget *ttv;
    //GtkWidget *etv;
//} cb_data; 

typedef struct app_objects {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *header;
    GtkWidget *etv;
    GtkWidget *ttv;
    GtkWidget **os_buttons;
    GtkWidget *filename_entry;

    ImageInfo *user_info;
} app_objects;
