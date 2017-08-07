#ifndef BINARY_INTERFACE_H
#define BINARY_INTERFACE_H

#include "appdefs.h"

int format_target_device(char *blockdev, char *format);
int mount_target_device(char *blockdev);
int create_forensic_image(app_objects *globals);

#endif /* BINARY_INTERFACE_H */
