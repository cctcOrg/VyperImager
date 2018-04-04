#ifndef BINARY_INTERFACE_H
#define BINARY_INTERFACE_H

#include "infocont.h"

namespace bint
{
    /* Takes no arguments and returns 1 to indicate that there is a 
     * devices mounted at /media/EVID_TARGET, or 0 otherwise */
    int target_is_mounted();

    /* Using the device path and the desired filesystem, clear the existing
     * partition table and create a new partition table with a single partition */
    void partition_device(string &devpath, string &fs);

    /* Takes a pointer to blockdev string, and another to a string 
     * that indicates what filesystem should be created. It returns a pointer to an
     * array, terminated by -1, that represents the arguments for a subprocess to
     * be created with */
    char **format_target_device(string &blockdev, string &format);

    /* Takes the name of a blockdev (e.g. "sda") and construct a command to mount 
     * the associated path (e.g. "/dev/sda") at /media/EVID_TARGET. Will return
     * null if the /media/EVID_TARGET is already mounted */
    char **mount_target_device(string &blockdev);

    /* Use the results from all of the information gathered to construct a commmand
     * array to be run as a subprocess */
    char **create_forensic_image(InfoCont &i);

    /* Take the name of a blockdev and writeblock it by executing `blockdev --setro
     * /dev/<name>` TODO: check if it's writeblocked first */
    int writeblock_evidence_device(string &dev);

    /* Create /media/EVID_TARGET if it does not exist */
    int create_mountpoint();

    /* Ensure /media/EVID_TARGET is not mounted */
    void unmount_target();
}

#endif /* BINARY_INTERFACE_H */
