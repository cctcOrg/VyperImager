#ifndef DEVINFO_H
#define DEVINFO_H

#define HIDE_INT 1
#define SHOW_INT 0

typedef struct Device {
    char *name;
    char *model;
    char *size;
    int  removable;
    int  is_target;
} Device;

Device *get_blockdev_struct(char *name);
Device **get_blockdev_info();

#endif /* DEVINFO_H */
