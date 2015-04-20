#ifndef __USB_QUEUE_H
#define __USB_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include <mcs.h>
#include <sdb.h>

#define USB_QUEUE_SIZE 128

typedef struct QueueElement { 
    MCSPacket * data; 
    int id_process; 
    int priority;
} QueueElement;

typedef struct USBQueue { 
    QueueElement *buf; 
    int n, alloc; 
} USBQueue;

extern USBQueue queue;

extern pthread_mutex_t usb_queue_lock;

extern sem_t packet_queue_count;

void usb_queue_init(void);

void usb_queue_push(MCSPacket *packet, int id_process);

MCSPacket *usb_queue_pop(int *id_process);

#endif /* __USB_QUEUE_H */