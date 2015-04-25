#ifndef __USB_QUEUE_H
#define __USB_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include <mcs.h>
#include <sdb.h>
#include "sdb_private.h"

#define USB_QUEUE_SIZE 128

typedef struct QueueElement { 
    int id_process; 
    int priority;
    const MCSPacket * data; 
} QueueElement;

typedef struct USBQueue { 
    QueueElement *buf; 
    int n, alloc; 
} USBQueue;

extern USBQueue queue;

extern pthread_mutex_t usb_queue_lock;

extern sem_t packet_queue_count;

void usb_queue_init(void);

void usb_queue_push(const struct MCSPacket *packet, int id_process);

const struct MCSPacket *usb_queue_pop(int *id);

#endif /* __USB_QUEUE_H */