#include "devinfo.h"

Device **get_blockdev_info(int *num_blockdevs) {
    Device **device_info = malloc(5*sizeof(Device*));

    DIR *dir;
    struct dirent *ent;
    int i = 0;

    /* Find the block devices */
    if ((dir = opendir("/sys/block")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                device_info[i] = get_blockdev_struct(ent->d_name); 
                i++;
            }
        }
        closedir(dir);
    } 
    else {
        perror("");
        return NULL;
    }    
    
    /*for (int j=0; j<i; j++)*/
        /*printf("Device: %s\nModel: %sSize: %s\n", */
                /*device_info[j]->name, device_info[j]->model, device_info[j]->size);*/

    *num_blockdevs = i;
    return device_info;
}

Device *get_blockdev_struct(char *name) {
    FILE *info_file;
    char info_path[40];
    
    char *devname = malloc(strlen(name)*sizeof(char));
    char *model = malloc(40*sizeof(char));

    char *size = malloc(15*sizeof(char));
    char size_raw[15];
    float size_value;
    char *sz_end;

    char rem[10]; 

    Device *devinfo = malloc(sizeof(Device));

    strcpy(devname, name);

    sprintf(info_path, "/sys/block/%s/device/model", name);
    info_file = fopen(info_path, "r");
    fgets(model, 40, info_file);
    /* Remove trailing \n */
    model[strlen(model)-1] = 0;
    fclose(info_file);

    sprintf(info_path, "/sys/block/%s/size", name);
    info_file = fopen(info_path, "r");
    fgets(size_raw, 15, info_file);
    fclose(info_file);

    size_raw[strlen(size_raw)-1] = 0;
    size_value = (float) strtol(size_raw, &sz_end, 10);
    size_value /= 1000000;
    sprintf(size, "%.2f", size_value);
    strcat(size, " G");

    sprintf(info_path, "/sys/block/%s/removable", name);
    info_file = fopen(info_path, "r");
    fgets(rem, 10, info_file);
    fclose(info_file);

    devinfo->name = devname;
    devinfo->model = model;
    devinfo->size = size;
    devinfo->removable = (rem[0] == '0') ? 0 : 1;

    return devinfo;
}
