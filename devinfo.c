#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>
#include "devinfo.h"

#define BAD_STAT -1
#define NUM_BLOCKDEVS 5
#define MAX_BLOCKLEN 15
#define INODE_OFFSET 5
#define SIZE_SIZE 15
#define PATH_SIZE 40
#define MODEL_SIZE 60
#define REM_SIZE 10

static int get_inode(char *filename) {
    struct stat file_stat;  
    int ret;  

    ret = stat(filename, &file_stat);  
    if (ret < 0)
        return BAD_STAT;

    return file_stat.st_ino;
}

Device **get_blockdev_info(int *num_blockdevs) {
    DIR *dir;
    struct dirent *ent;
    int i = 0;
    char *fn;
    char test_path[MAX_BLOCKLEN];
    int target_inode;
    int inode;

    Device *dev;
    Device **device_info = malloc(NUM_BLOCKDEVS*sizeof(Device*));

    target_inode = get_inode("/dev/disk/by-label/EVID_TARGET");

    /* Find the block devices */
    if ((dir = opendir("/sys/block")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            fn = ent->d_name;
            if (fn[0] != '.') {
                dev = get_blockdev_struct(fn);
                dev->is_target = 0;

                sprintf(test_path, "/dev/%s", fn);
                inode = get_inode(test_path);
                /* If this is our dear evidence device, the data partition's
                 * inode will have a small offset from the block device's */ 
                if (abs(inode-target_inode) < INODE_OFFSET)
                    dev->is_target = 1;

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

Device *get_blockdev_struct(char *name) {
    FILE *info_file;
    char info_path[PATH_SIZE];
    
    /* strlen doesn't count the null character, need to do that */
    char *devname = malloc((strlen(name)+1)*sizeof(char));
    char *model = malloc(MODEL_SIZE*sizeof(char));

    char *size = malloc(SIZE_SIZE*sizeof(char));
    char size_raw[SIZE_SIZE];
    float size_value;
    char *sz_end;

    char rem[REM_SIZE]; 

    Device *devinfo = malloc(sizeof(Device));

    strcpy(devname, name);

    sprintf(info_path, "/sys/block/%s/device/model", name);
    
    /* Model */
    info_file = fopen(info_path, "r");
    if (info_file == NULL) {
        printf("Note: no model: %s\n", name);
        strcpy(model, "SD Card");
    } 
    else {
        fgets(model, MODEL_SIZE, info_file);
        /* Remove trailing \n */
        model[strlen(model)-1] = 0;
        fclose(info_file);
    }

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
    size_value = (float) strtol(size_raw, &sz_end, SIZE_SIZE);
    size_value /= 1000000;
    sprintf(size, "%.2f", size_value);
    strcat(size, " G");

    /* Removable */
    sprintf(info_path, "/sys/block/%s/removable", name);
    info_file = fopen(info_path, "r");
    fgets(rem, REM_SIZE, info_file);
    fclose(info_file);

    devinfo->name = devname;
    devinfo->model = model;
    devinfo->size = size;
    devinfo->removable = (rem[0] == '0') ? 0 : 1;

    return devinfo;
}

