#ifndef DEVINFO_H
#define DEVINFO_H

#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>

#define HIDE_INT 1
#define SHOW_INT 0

typedef struct Device {
    char *name;
    char *model;
    char *size;
    int  removable;
} Device;

Device *get_blockdev_struct(char *name);
Device **get_blockdev_info();

#endif /* DEVINFO_H */
