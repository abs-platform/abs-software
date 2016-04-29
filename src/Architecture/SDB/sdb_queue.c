#include "sdb_private.h"

#define SDB_PACKET_DEFAULT_PRIO 0

static void sdb_queue_delete(SDBQueue *queue, SDBQueueElem *queue_elem)
{
    if (queue_elem->prev != NULL) {
        queue_elem->prev->next = queue_elem->next;
    } else {
        /* queue was the first. Now the first is the next */
        queue->queue_first = queue_elem->next;
    }

    if (queue_elem->next == NULL) {
        /* queue was the last. Now the last is queue_prev */
        queue->queue_last = queue_elem->prev;
    } else {
        queue_elem->next->prev = queue_elem->prev;
    }

    free(queue_elem);
}

/* Packet management */
SDBPacket *sdb_packet(MCSPacket *pkt, unsigned int id)
{
    SDBPacket *sdb_pkt = malloc(sizeof(*sdb_pkt));

    sdb_pkt->id_process = id;
    sdb_pkt->priority = SDB_PACKET_DEFAULT_PRIO;
    sdb_pkt->pkt = pkt;

    return sdb_pkt;
}

SDBPacket *sdb_packet_prio(MCSPacket *pkt, unsigned int id)
{
    SDBPacket *sdb_pkt = malloc(sizeof(*sdb_pkt));

    sdb_pkt->id_process = id;
    sdb_pkt->priority = sdb_group_priority[sdb_module[id].group];
    sdb_pkt->pkt = pkt;

    return sdb_pkt;
}

void sdb_packet_free(SDBPacket *sdb_pkt)
{
    mcs_free(sdb_pkt->pkt);
    free(sdb_pkt);
}

/* Queue management */
/* Current implementation uses a linked list. This makes insert, delete and
 * pop of cost O(1), and search of cost O(n).
 */

/* Lock the queue and push */
void sdb_queue_push(SDBQueue *queue, SDBPacket *sdb_pkt)
{
    pthread_mutex_lock(&queue->queue_lock);
    sdb_queue_push_nolock(queue, sdb_pkt);
    pthread_cond_broadcast(&queue->queue_var);
    pthread_mutex_unlock(&queue->queue_lock);
}

/* Push without locking the queue */
void sdb_queue_push_nolock(SDBQueue *queue, SDBPacket *sdb_pkt)
{
    SDBQueueElem *queue_elem = malloc(sizeof(*queue_elem));
    SDBQueueElem *queue_tmp;

    queue_elem->pkt = sdb_pkt;
    queue_elem->prev = NULL;
    queue_elem->next = NULL;

    if (queue->queue_first == NULL) {
        queue->queue_first = queue_elem;
    } else {
        queue_elem->prev = queue->queue_last;
        queue->queue_last->next = queue_elem;
    }

    queue->queue_last = queue_elem;

    while (queue_elem->prev != NULL &&
            queue_elem->pkt->priority > queue_elem->prev->pkt->priority) {
        /* Swap */
        queue_tmp = queue_elem->prev;

        queue_elem->prev = queue_tmp->prev;
        if (queue_elem->prev == NULL) {
            queue->queue_first = queue_elem;
        } else {
            queue_elem->prev->next = queue_elem;
        }

        queue_tmp->next = queue_elem->next;
        if (queue_tmp->next == NULL) {
            queue->queue_last = queue_tmp;
        } else {
            queue_tmp->next->prev = queue_tmp;
        }

        queue_tmp->prev = queue_elem;
        queue_elem->next = queue_tmp;
    }
}

/* Lock the queue and get the packet that originated the answer */
SDBPacket *sdb_queue_get(SDBQueue *queue, MCSPacket *answer)
{
    SDBPacket *res;

    pthread_mutex_lock(&queue->queue_lock);
    res = sdb_queue_get_nolock(queue, answer);
    pthread_mutex_unlock(&queue->queue_lock);
    return res;
}

/* Get the packet that originated the answer without locking the queue */
SDBPacket *sdb_queue_get_nolock(SDBQueue *queue, MCSPacket *answer)
{
    SDBPacket *res;
    SDBQueueElem *queue_elem = queue->queue_first;

    while (queue_elem != NULL && queue_elem->pkt->pkt->id != answer->id) {
        queue_elem = queue_elem->next;
    }

    if (queue_elem == NULL) {
        return NULL;
    } else {
        res = queue_elem->pkt;
        sdb_queue_delete(queue, queue_elem);
        return res;
    }
}

/* Block until it is possible to pop from the queue */
SDBPacket *sdb_queue_pop_block(SDBQueue *queue)
{
    SDBPacket *res;
    SDBQueueElem *queue_elem;

    pthread_mutex_lock(&queue->queue_lock);
    queue_elem = queue->queue_first;

    if (queue_elem == NULL) {
        pthread_cond_wait(&queue->queue_var, &queue->queue_lock);
    }

    res = queue_elem->pkt;
    sdb_queue_delete(queue, queue_elem);
    pthread_mutex_unlock(&queue->queue_lock);
    return res;
}

void sdb_queue_init(SDBQueue *queue)
{
    pthread_mutex_init(&queue->queue_lock, NULL);
    pthread_cond_init(&queue->queue_var, NULL);

    queue->queue_first = NULL;
    queue->queue_last = NULL;
}
