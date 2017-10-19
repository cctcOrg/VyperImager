#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>

#include<sys/stat.h>
#include<sys/ioctl.h>
#include<linux/fs.h>

#include "binary_interface.h"

#define MAX_MOUNT_LINE 255
#define FORMAT_CMD_ARGC 8

/* Thanks to ewg on ##c for this! */
int target_is_mounted()
{
    FILE *f = fopen("/proc/mounts", "r");
    if (!f)
        return -1;

    char *buf = NULL;
    while (fscanf(f, "%*s %ms %*s %*s %*s %*s\n", &buf) == 1)
        if (strstr(buf, "/media/EVID_TARGET") == buf)
            return 1;

    return 0;
}

int writeblock_evidence_device(char *dev)
{
    char command[40];
    char command_tmp[40];
    strcpy(command_tmp, "blockdev --setro /dev/%s");
    sprintf(command, command_tmp, dev);

    return system(command);
}

/* Note: Need to create a partition table with a partiton */
char **format_target_device(char *blockdev, char *format)
{
    char **cmd_array;
    char *label_flag = "-L \"EVID_TARGET\" -F";
    char *hfsp_label_flag = "-v \"EVID_TARGET\"";

    char *cmd_format = "mkfs -t %s %s /dev/%s1";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + strlen(format) 
             + strlen(label_flag) + 1) * sizeof(char));

    sprintf(cmd_string, cmd_format, format, 
            (format[0] == 'h') ? hfsp_label_flag : label_flag, blockdev);

    g_shell_parse_argv(cmd_string, NULL, &cmd_array, NULL);

    free(cmd_string);
    return cmd_array;
}

char **mount_target_device(char *blockdev)
{
    /* Wow look, my work might already be done! */
    if (target_is_mounted() == 1)
        return 0;

    DIR *dir = opendir("/media");
    if (dir)
        closedir(dir);
    else
        mkdir("/media", ACCESSPERMS);

    char *mountpoint = "/media/EVID_TARGET";
    /* See if mountpoint exists, create it if not */
    dir = opendir(mountpoint);
    if (dir)
        closedir(dir);
    else
        mkdir(mountpoint, ACCESSPERMS);

    char **cmd_array;
    char *cmd_format = "mount /dev/%s1 /media/EVID_TARGET";
    char *cmd_string = malloc( 
            (strlen(cmd_format) + strlen(blockdev) + 1) * sizeof(char));

    sprintf(cmd_string, cmd_format, blockdev);
    g_shell_parse_argv(cmd_string, NULL, &cmd_array, NULL);
    free(cmd_string);
    return cmd_array; 
}

char **create_forensic_image(app_objects *globals)
{
    ImageInfo *info = globals->user_info;
    Case *cinfo = info->case_info;


    char **cmd_array;
    /*char *cmd_format = "mount /dev/%s1 /media/EVID_TARGET";*/
    /*char *cmd_string = malloc( */
            /*(strlen(cmd_format) + strlen(blockdev) + 1) * sizeof(char));*/

    /*sprintf(cmd_string, cmd_format, blockdev);*/
    /*g_shell_parse_argv(cmd_string, NULL, &cmd_array, NULL);*/
    /*free(cmd_string);*/
    return cmd_array; 
}
