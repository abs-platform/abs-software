#include "usb_queue.h"

USBQueue queue;
sem_t packet_queue_count;
pthread_mutex_t usb_queue_lock;
int ids = 0;

SDBModule sdb_module[SDB_MODULE_MAX];

void usb_queue_init(void)
{
    /* Initialize the usb queue */
    queue.buf = malloc(sizeof(QueueElement) * USB_QUEUE_SIZE);
    queue.alloc = USB_QUEUE_SIZE;
    queue.n = 0;
    pthread_mutex_init(&usb_queue_lock, NULL);
    /* Initialize the semaphore which counts packets in
    the usb queue.Its initial value should be zero. */
    sem_init(&packet_queue_count, 0, 0);
    return;
}

static void swap(USBQueue *q, int i, int j) 
{
    QueueElement temp = q->buf[i];
    q->buf[i] = q->buf[j];
    q->buf[j] = temp;
}

void usb_queue_push(const struct MCSPacket *packet, int id_process)
{
    int i = 0, n = 0;
    QueueElement *b;
    /* Lock the mutex on the usb queue. */
    pthread_mutex_lock(&usb_queue_lock);
    /* Add the element and reorder the queue. */
    b = queue.buf;   
 
    b[n].data = abs_malloccpy(packet, sizeof(*packet));

    if(b[n].data->dest != NULL) {
        ((MCSPacket *)(b[n].data))->dest = abs_malloccpy(b[n].data->dest, strlen(b[n].data->dest));    
    }

    if(b[n].data->nargs != 0) {
        ((MCSPacket *)(b[n].data))->args = abs_malloccpy(b[n].data->args, b[n].data->nargs);
    }
    if(b[n].data->data_size != 0) {
        ((MCSPacket *)(b[n].data))->data = abs_malloccpy(b[n].data->data, b[n].data->data_size);
    }

    b[n].priority = sdb_group_priority[sdb_module[id_process].group];
    ids = id_process;
    for(i = n; (i > 1) && (b[i].priority < b[i-1].priority); i--) {
        swap(&queue, i, i-1);
    }
    n = queue.n++;
    /* Post to the semaphore to indicate another packet is available. */
    sem_post(&packet_queue_count);
    /* Unlock the usb queue mutex. */
    pthread_mutex_unlock(&usb_queue_lock);
}

const struct MCSPacket * usb_queue_pop(int *id)
{
    int i;
    QueueElement *b;
    /* If the queue is empty, block until a new packet is enqueued.  */
    sem_wait(&packet_queue_count);
    printf_dbg("stage1\n");
    /* Lock the mutex on the usb queue.  */
    pthread_mutex_lock(&usb_queue_lock);  
    if(queue.n == -1) {
        pthread_mutex_unlock(&usb_queue_lock); 
        return NULL;
    }
    /* Reorder the queue and pop the first element. */
    b = queue.buf;
    printf_dbg("stage2\n");
    queue.n--;
    *id = ids;
    b[0] = b[queue.n];
    for(i = 1; i <= queue.n; i++) {
        swap(&queue, i, i-1);
    }
    /* Unlock the usb queue mutex. */
    pthread_mutex_unlock(&usb_queue_lock); 
    printf_dbg("stage3\n");
    return b[0].data;
}