#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include<parted/device.h>
#include<parted/disk.h>

#include "binary_interface.h"

#define MAX_MOUNT_LINE 255

/* Thanks to ewg on ##c for this! */
static int target_is_mounted() {
    FILE *f = fopen("/proc/mounts", "r");
    if (!f)
        return -1;

    char *buf = NULL;
    while (fscanf(f, "%*s %ms %*s %*s %*s %*s\n", &buf) == 1)
        if (strstr(buf, "/media/EVID_TARGET") == buf)
            return 1;

    return 0;
}

/* Note: Need to create a partition table with a partiton */
int format_target_device(char *blockdev, char *format) {
    char *label_flag = "-L \"EVID_TARGET\" -F";
    char *hfsp_label_flag = "-v \"EVID_TARGET\"";

    char *cmd_format = "mkfs -t %s %s /dev/%s1";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + strlen(format) 
             + strlen(label_flag) + 1) * sizeof(char));

    sprintf(cmd_string, cmd_format, format, 
            (format[0] == 'h') ? hfsp_label_flag : label_flag, blockdev);
    
    int result = system(cmd_string);
    free(cmd_string);
    return result;
}

/* Returns 0 on success, failure otherwise */
int mount_target_device(char *blockdev) {
    /* Wow look, my work might already be done! */
    if (target_is_mounted() == 1)
        return 0;

    char *mountpoint = "/media/EVID_TARGET";

    /* See if mountpoint exists, create it if not */
    DIR *dir = opendir("/media/EVID_TARGET");
    if (dir)
        closedir(dir);
    else
        mkdir(mountpoint, ACCESSPERMS);


    char *cmd_format = "mount /dev/%s1 /media/EVID_TARGET";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + 1) * sizeof(char));

    sprintf(cmd_string, cmd_format, blockdev);
    int result = system(cmd_string);
    free(cmd_string);
    return result; 
}

int create_forensic_image(app_objects *globals) {
    return 0;
}
