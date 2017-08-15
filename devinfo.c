#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>

#include<parted/device.h>
#include<blkid/blkid.h>

#include "devinfo.h"

#define BAD_STAT -1
#define NUM_BLOCKDEVS 5
#define MAX_BLOCKLEN 15
#define INODE_OFFSET 5
#define SIZE_SIZE 15
#define PATH_SIZE 40
#define MODEL_SIZE 60
#define REM_SIZE 10

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
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    char *fn;
    char *target_device;

    Device *dev;
    Device **device_info = malloc(NUM_BLOCKDEVS*sizeof(Device*));

    target_device = get_target_dev("/dev/disk/by-label/EVID_TARGET");

    /* Find the block devices */
    if ((dir = opendir("/sys/block")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            fn = ent->d_name;
            /* Don't include "." entries or loop devices */
            if (fn[0] != '.' && fn[0] != 'l') {
                dev = get_blockdev_struct(fn);
                if (target_device != NULL)
                    dev->is_target = (strcmp(target_device, fn) == 0) ? 1: 0;
                device_info[i] = dev;
                i++;
            }
        }
        closedir(dir);
    } 
    else {
        perror("");
        return NULL;
    }    
    
    *num_blockdevs = i;
    return device_info;
}

Device *get_blockdev_struct(PedDevice *dev) {
    blkid_probe prober;
    char *tmp;
    char **labels;
    int numparts;

    blkid_new_probe_from_filename(dev->path);
    numparts = blkid_partlist_numof_partitions(blkid_probe_get_partitions(probe));
    labels = malloc(numparts*sizof(char*));

    FILE *info_file;
    char info_path[PATH_SIZE];
    char *dev_tmp;
    
    /* strlen doesn't count the null character, need to do that */
    char *dev_path = malloc((strlen(dev->path)+1)*sizeof(char));
    strcpy(dev_path, dev->path);
    strtok(dev_path, "/"); // "dev"
    dev_tmp = strtok(NULL, "/"); // <name>

    char *devname = malloc((strlen(name)+1)*sizeof(char));
    strcpy(devname, dev_tmp);
    /*char *model = malloc(MODEL_SIZE*sizeof(char));*/

    char *size = malloc(SIZE_SIZE*sizeof(char));
    char size_raw[SIZE_SIZE];
    float size_value;
    char *sz_end;

    char rem[REM_SIZE]; 

    Device *devinfo = malloc(sizeof(Device));

    /*sprintf(info_path, "/sys/block/%s/device/model", name);*/
    
    /* Model */
    /*info_file = fopen(info_path, "r");*/
    /*if (info_file == NULL) {*/
        /*printf("Note: no model: %s\n", name);*/
        /*strcpy(model, "SD Card");*/
    /*} */
    /*else {*/
        /*fgets(model, MODEL_SIZE, info_file);*/
        /*[> Remove trailing \n <]*/
        /*model[strlen(model)-1] = 0;*/
        /*fclose(info_file);*/
    /*}*/

    /* Size */
    sprintf(info_path, "/sys/block/%s/size", name);

    info_file = fopen(info_path, "r");
    if (info_file == NULL) {
        printf("No such file or directory: %s\n", info_path);
        return devinfo;
    }

    fgets(size_raw, SIZE_SIZE, info_file);
    fclose(info_file);
    
    size_raw[strlen(size_raw)-1] = 0;
    size_value = (float) strtol(size_raw, &sz_end, 10);
    /* File gives value in 512-byte sectors, need it in bytes */
    size_value *= 512;
    /* Convert to GB */
    size_value /= 1000000000;

    sprintf(size, "%.2f", size_value);
    strcat(size, " G");

    /* Removable */
    sprintf(info_path, "/sys/block/%s/removable", name);
    info_file = fopen(info_path, "r");
    fgets(rem, REM_SIZE, info_file);
    fclose(info_file);
    
    devinfo->dev = dev;
    devinfo->name = devname;
    devinfo->model = model;
    devinfo->size = size;
    devinfo->numparts = numparts;
    devinfo->removable = (rem[0] == '0') ? 0 : 1;

    return devinfo;
}

