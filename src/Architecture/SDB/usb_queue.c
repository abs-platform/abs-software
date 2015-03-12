#include "usb_queue.h"
 
UsbQueue queue;
sem_t packet_queue_count;
pthread_mutex_t usb_queue_lock;

void usb_queue_init()
{
    /* Initialize the usb queue */
    queue = malloc(sizeof(UsbQueueT));
    queue->buf = malloc(sizeof(QueueElem) * MAX_SIZE);
    queue->alloc = MAX_SIZE;
    queue->n = 1; 
    pthread_mutex_init(&usb_queue_lock, NULL);
    /* Initialize the semaphore which counts packets in
    the usb queue.Its initial value should be zero. */
    sem_init(&packet_queue_count, 0, 0);
    return;
}

void swap(UsbQueue q, int i, int j) 
{
    QueueElem temp = q->buf[i];
    q->buf[i] = q->buf[j];
    q->buf[j] = temp;
}
 
void usb_queue_push(void *data, int priority)
{
    int i,n;
    QueueElem *b;
    /* Lock the mutex on the usb queue. */
    pthread_mutex_lock(&usb_queue_lock);
    /* Add the element and reorder the queue. */
    b = queue->buf;
    n = queue->n++;
    b[n].data = data;
    b[n].priority = priority;
    for(i = n;( i > 1) && (b[i].priority > b[i-1].priority); i--) {
        swap(queue,i,i-1);
    }
    /* Post to the semaphore to indicate another packet is available. */
    sem_post(&packet_queue_count);
    /* Unlock the usb queue mutex. */
    pthread_mutex_unlock(&usb_queue_lock);
}
 
void * usb_queue_pop(int *priority)
{
    int i;
    void *out;
    /* If the queue is empty, block until a new packet is enqueued.  */
    sem_wait(&packet_queue_count);
    /* Lock the mutex on the usb queue.  */
    pthread_mutex_lock(&usb_queue_lock);  
    if (queue->n == 1) {
        pthread_mutex_unlock(&usb_queue_lock); return NULL;
    }
    /* Reorder the queue and pop the first element. */
    QueueElem * b = queue->buf;
    out = b[1].data;
    if (priority) * priority = b[1].priority;
    b[1]=b[--queue->n];
    for(i=1; i<=queue->n;i++) {
        swap(queue,i,i-1);
    }
    if(queue->n < queue->alloc / 2 && queue->n >= 16) {
        queue->buf = realloc(queue->buf, (queue->alloc /= 2) * sizeof(b[0]));
    }
    /* Unlock the usb queue mutex. */
    pthread_mutex_unlock(&usb_queue_lock); 
    return out;
}
  