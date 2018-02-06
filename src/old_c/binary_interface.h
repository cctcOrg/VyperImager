#ifndef BINARY_INTERFACE_H
#define BINARY_INTERFACE_H

#include "appdefs.h"

/* Takes no arguments and returns 1 to indicate that there is a 
 * devices mounted at /media/EVID_TARGET, or 0 otherwise */
int target_is_mounted();

/* Takes a pointer to blockdev string, and another to a string 
 * that indicates what filesystem should be created. It returns a pointer to an
 * array, terminated by -1, that represents the arguments for a subprocess to
 * be created with */
char **format_target_device(char *blockdev, char *format);

/* Takes the name of a blockdev (e.g. "sda") and construct a command to mount 
 * the associated path (e.g. "/dev/sda") at /media/EVID_TARGET. Will return
 * null if the /media/EVID_TARGET is already mounted */
char **mount_target_device(char *blockdev);

/* Take the name of a blockdev and writeblock it by executing `blockdev --setro
 * /dev/<name>` TODO: check if it's writeblocked first */
int writeblock_evidence_device(char *dev);

/* Use the results from all of the information gathered to construct a commmand
 * array to be run as a subprocess */
char **create_forensic_image(app_objects *globals);

#endif /* BINARY_INTERFACE_H */
