#ifndef USB_QUEUE_H
#define USB_QUEUE_H

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "sdb_dummy.h" //sdb.h

#define MAX_SIZE 128

typedef struct { 
	void * data; 
	int priority; 
} QueueElem;

typedef struct { 
	QueueElem *buf; 
	int n, alloc; 
} *UsbQueue, UsbQueueT;

extern usb_queue queue;

extern pthread_mutex_t usb_queue_lock;

extern sem_t packet_queue_count;

void usb_queue_init();

void usb_queue_push(void *data, int pri);

void *usb_queue_pop(int *pri);

#endif /* USB_QUEUE_H */