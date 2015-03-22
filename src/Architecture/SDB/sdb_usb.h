#ifndef SDB_USB_H
#define SDB_USB_H

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "usb_queue.h"

#define THE_DEVICE "/dev/usb_accessory"

void* sdb_usb(void *arg);

#endif /* SDB_USB_H */
