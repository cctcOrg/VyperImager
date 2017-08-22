#ifndef DEVINFO_H
#define DEVINFO_H

#define HIDE_INT 1
#define SHOW_INT 0

#include<parted/device.h>

typedef struct Device {
    PedDevice *dev;
    char *name;
    char *model;
    char *size;
    char **labels;
    int  numparts;
    int  removable;
    int  is_target;
} Device;

/* Will use parted to probe for all block devices and then construct Device
 * structures for each one, storing the number of found block devices in the
 * given num_blockdevs output parameter. Returns a newly allocated array of 
 * Devices of the size indicated by the retuned num_blockdevs*/
Device **get_blockdev_info(int *num_blockdevs);

/* Uses a combination of blkid and parted to probe a given device for information
 * so that the Device struct can be populated. Returns a newly allocated Device
 * struct */
Device *get_blockdev_struct(PedDevice *dev);

#endif /* DEVINFO_H */
