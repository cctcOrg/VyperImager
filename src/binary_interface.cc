extern "C"
{
#include<glib.h>
#include<dirent.h>

#include<sys/stat.h>
#include<sys/ioctl.h>
#include<linux/fs.h>
}


#include <cstdlib>
#include <cstring>
#include <sstream>
#include <vector>
#include "binary_interface.h"

#define MAX_MOUNT_LINE 255
#define FORMAT_CMD_ARGC 8

using namespace std;

/* Thanks to ewg on ##c for this! */
int bint::target_is_mounted()
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

int bint::writeblock_evidence_device(string dev)
{
    stringstream command;
    command << "blockdev --setro /dev/" << dev;
    return std::system(command.str().c_str());
}

/* Note: Need to create a partition table with a partiton */
char** bint::format_target_device(string blockdev, string format)
{
    char** cmd_array;
    string label_flag = "-L \"EVID_TARGET\" -F";
    string hfsp_label_flag = "-v \"EVID_TARGET\"";

    stringstream command; 

    command <<  "mkfs -t " << format;
    if (format[0] == 'h') 
        command << hfsp_label_flag;
    else
        command << label_flag;
    command << blockdev;

    g_shell_parse_argv(command.str().c_str(), NULL, &cmd_array, NULL);

    return cmd_array;
}

char** bint::mount_target_device(string blockdev)
{
    /* Wow look, my work might already be done! */
    if (bint::target_is_mounted() == 1)
        return nullptr;

    DIR *dir = opendir("/media");
    if (dir)
        closedir(dir);
    else
        mkdir("/media", ACCESSPERMS);

    string mountpoint = "/media/EVID_TARGET";
    /* See if mountpoint exists, create it if not */
    dir = opendir(mountpoint.c_str());
    if (dir)
        closedir(dir);
    else
        mkdir(mountpoint.c_str(), ACCESSPERMS);

    char **cmd_array;
    stringstream command; 
    command << "mount /dev/" << blockdev << "1 /media/EVID_TARGET";

    g_shell_parse_argv(command.str().c_str(), NULL, &cmd_array, NULL);
    return cmd_array; 
}

char** bint::create_forensic_image(InfoCont &i)
{
    char **cmd_array;
    stringstream command;
    string ct = g_ascii_strdown(i.compression_type.c_str(), -1);
    string dt = g_ascii_strdown(i.device_type.c_str(), -1);
    /* Command format:
     *  -t: target file, without extension
     *  -C: case number
     *  -E: evidence number
     *  -e: examiner name
     *  -D: description
     *  -N: notes
     *  -m: media type
     *  -d: digest type (sha1/sha256)
     *  -c: compression values (just the level)
     *  -l: log filename
     *  Final arg: the device to image
     *  */
    
    // Old format string for reference
    //char *cmd_format = "ewfacquire -u -t %s/%s -C \"%s\" -E \"%s\" -e \"%s\" "
       //"-D \"%s\" -N \"%s\" -m %s -d \"%s\" -c %s "
       //"-l aquisition.log /dev/%s "; 
    
    command << "ewfacquire -u -t " + i.target_directory + "/" + i.target_filename
        + " -C \"" + i.casenum + "\" -E \"" + i.itemnum + "\" -e \"" + i.examiner
        + " -D \"" + i.desc + "\" -N \"" + i.notes + "\" -m " + dt
        + " -d \"" + i.hash_type + " -c " + ct + " -l aquisition.log /dev/"
        + i.evidence_device;

    g_shell_parse_argv(command.str().c_str(), NULL, &cmd_array, NULL);

    return cmd_array; 
}
