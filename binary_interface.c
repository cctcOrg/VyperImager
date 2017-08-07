#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>

#include "binary_interface.h"

int format_target_device(char *blockdev, char *format) {
    char *cmd_format = "mkfs -t %s /dev/%s";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + strlen(format) + 1)
            * sizeof(char));

    sprintf(cmd_string, cmd_format, format, blockdev);
    int result = system(cmd_string);
    free(cmd_string);
    return result;
}

int mount_target_device(char *blockdev) {
    char *mountpoint = "/media/EVID_TARGET"; 

    /* See if mountpoint exists, create it if not */
    DIR *dir = opendir("/media/EVID_TARGET");
    if (dir)
        closedir(dir);
    else
        mkdir(mountpoint, ACCESSPERMS);


    char *cmd_format = "mount /dev/%s /media/EVID_TARGET";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + 1) * sizeof(char));

    sprintf(cmd_string, cmd_format, blockdev);
    int result = system(cmd_string);
    free(cmd_string);
    return result; 
}

int create_forensic_image(app_objects *globals) {
    ;
}
