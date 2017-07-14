typedef struct TargetData {
    char *directory;
    char *filename;
} TargetData;

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
    TargetData *target_info;
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

