#ifndef BINARY_INTERFACE_H
#define BINARY_INTERFACE_H

#include "appdefs.h"

static int target_is_mounted();
char **format_target_device(char *blockdev, char *format);
char **mount_target_device(char *blockdev);
int create_forensic_image(app_objects *globals);
int writeblock_evidence_device(char *dev);

#endif /* BINARY_INTERFACE_H */
