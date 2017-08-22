#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>

#include<parted/device.h>
#include<blkid/blkid.h>

#include "devinfo.h"

#define NUM_BLOCKDEVS 5
#define SIZE_SIZE 15
#define PATH_SIZE 40
#define MODEL_SIZE 60
#define REM_SIZE 10

/* Takes a path, resolving any symbolic links, and strips away everything before
 * and including the second '/' found -- for example /dev/mmcblk0 -> mmcblk0.
 * Note that this function will mangle the given filename */
static char *get_target_dev(char *filename) {
    char *actualpath;
    char *token;
    
    actualpath = realpath(filename, NULL);
    if (actualpath) {
        token = strtok(actualpath, "/"); // Should be "dev"
        token = strtok(NULL, "/"); // should be block device
    
        /* Chop the partition number off so we just have the device */
        token[strlen(token)-1] = 0;
        /* Since token points to data in actualpath, which is dynamically 
         * allocated, this should work */
        return token;
    }

    else
        return NULL;
}

Device **get_blockdev_info(int *num_blockdevs) {
    PedDevice *ped = NULL;
    char *target_device;
    int i = 0;

    Device *dev;
    Device **device_info = malloc(NUM_BLOCKDEVS*sizeof(Device*));
    Device **devinf_real;

    target_device = get_target_dev("/dev/disk/by-label/EVID_TARGET");
    
    ped_device_probe_all();

    while ( (ped = ped_device_get_next(ped)) ) {
        dev = get_blockdev_struct(ped);
        if (target_device != NULL)
            dev->is_target = (strcmp(target_device, dev->name) == 0) ? 1: 0;
        device_info[i++] = dev;
    }

    *num_blockdevs = i;
    devinf_real = realloc(device_info, i*sizeof(Device*));
    /* If we couldn't make it smaller for some weird reason, then we'll just 
     * return the one that's too big */
    if (devinf_real == NULL)
        return device_info;
    else
        return devinf_real;
}

Device *get_blockdev_struct(PedDevice *dev) {
    FILE *info_file;
    char *tmp;
    const char *tmpc;
    blkid_partlist ls;
    char info_path[PATH_SIZE];
    char rem[REM_SIZE]; 
    char *devname;
    char **labels = NULL;
    char *wholedev_label;
    int numparts = 0;
    int i;
    int err;

    char *size = malloc(SIZE_SIZE*sizeof(char));
    float size_value;

    /* Literally just get /dev/<x> */
    char *dev_path = malloc((strlen(dev->path)+1)*sizeof(char));
    strcpy(dev_path, dev->path);
    strtok(dev_path, "/"); // "dev"
    tmp = strtok(NULL, "/"); // <name>
    devname = malloc((strlen(tmp)+1)*sizeof(char));
    strcpy(devname, tmp);
    free(dev_path);

    /* Find the size */
    blkid_probe pr = blkid_new_probe_from_filename(dev->path);
    size_value = blkid_probe_get_size(pr);
    size_value /= 1000000000;

    /* Let's see if the whole device has a label */
    blkid_do_probe(pr);
    err = blkid_probe_lookup_value(pr, "LABEL", &tmpc, NULL);

    if (err == 0) {
        wholedev_label = malloc(strlen(tmpc)+1);
        strcpy(wholedev_label, tmpc);
    }

    sprintf(size, "%.2f", size_value);
    strcat(size, " G");

    /* See what labels we can find */
    ls = blkid_probe_get_partitions(pr);
    if (ls != NULL) {
        i = 0;
        numparts = blkid_partlist_numof_partitions(ls);

        if (wholedev_label != NULL) {
            labels = malloc((numparts+1)*sizeof(char*));
            labels[i] = wholedev_label;
            i++;
        }
        else {
            labels = malloc(numparts*sizeof(char*));
        }

        for (i = 0; i < numparts; i++) {
            sprintf(info_path, "%s%d", dev->path, (i+1));

            pr = blkid_new_probe_from_filename(info_path);
            blkid_do_probe(pr);
            err = blkid_probe_lookup_value(pr, "LABEL", &tmpc, NULL);

            if (err == 0) {
                tmp = malloc(strlen(tmpc)+1);
                strcpy(tmp, tmpc);
                labels[i] = tmp;
            }
            else {
                /*printf("%s has no valid label\n", info_path);*/
                labels[i] = NULL;
            }

            blkid_free_probe(pr);
        }
    }

    /* Removable */
    sprintf(info_path, "/sys/block/%s/removable", devname);
    info_file = fopen(info_path, "r");
    fgets(rem, REM_SIZE, info_file);
    fclose(info_file);
    
    Device *devinfo = malloc(sizeof(Device));

    devinfo->dev = dev;
    devinfo->name = devname;
    devinfo->model = dev->model;
    devinfo->size = size;
    devinfo->numparts = numparts;
    devinfo->labels = labels;
    devinfo->removable = (rem[0] == '0') ? 0 : 1;

    return devinfo;
}

