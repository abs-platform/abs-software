#include "sdb_private.h"

/* Packet management */
SDBPacket *sdb_packet(MCSPacket *pkt, unsigned int id)
{
    SDBPacket *sdb_pkt = malloc(sizeof(*sdb_pkt));

    sdb_pkt->id_process = id;
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
void sdb_queue_push(SDBQueue *queue, SDBPacket *sdb_pkt)
{
    SDBQueueElem *queue_elem = malloc(sizeof(*queue_elem));

    queue_elem->pkt = sdb_pkt;
    queue_elem->next = NULL;

    if (queue->queue_first == NULL) {
        queue->queue_first = queue_elem;
    } else {
        queue->queue_last->next = queue_elem;
    }

    queue->queue_last = queue_elem;
}

SDBPacket *sdb_queue_get(SDBQueue *queue, MCSPacket *answer)
{
    SDBPacket *res;
    SDBQueueElem *queue_elem_prev = NULL;
    SDBQueueElem *queue_elem = queue->queue_first;

    while (queue_elem != NULL && queue_elem->pkt->pkt->id != answer->id) {
        queue_elem_prev = queue_elem;
        queue_elem = queue_elem->next;
    }

    if (queue_elem == NULL) {
        return NULL;
    }

    if (queue_elem_prev != NULL) {
        queue_elem_prev->next = queue_elem->next;
    } else {
        /* queue was the first. Now the first is tne next */
        queue->queue_first = queue_elem->next;
    }

    if (queue_elem->next == NULL) {
        /* queue was the last. Now the last is queue_prev */
        queue->queue_last = queue_elem_prev;
    }

    res = queue_elem->pkt;

    free(queue_elem);
    return res;
}

