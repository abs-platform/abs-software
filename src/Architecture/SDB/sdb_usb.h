#ifndef __SDB_USB_H
#define __SDB_USB_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcs.h>
#include <abs.h>
#include "sdb.h"
#include <sdb.h>
#include "usb_queue.h"

#define SDB_USB_DEVICE "/dev/usb_accessory"

#define MAX_SIZE_USB_PACKET 256

typedef enum {
    OK,
    OK_DATA,
    ERROR
} UsbResponse;

int init_sdb_usb(void);

#endif /* __SDB_USB_H */