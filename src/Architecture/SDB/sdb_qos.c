#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <abs.h>
#include "sdb_private.h"

#define SDB_QOS_TABLE_MAX 10

typedef enum SDBQOSEvent {
    SDB_QOS_PKT_IN,
    SDB_QOS_PKT_OUT,
    SDB_QOS_PKT_READY,
    SDB_QOS_PKT_SCRAP,
} SDBQOSEvent;

typedef struct SDBQOSMCS {
    unsigned int mcs_id;
    enum MCSType mcs_type;
    unsigned short mcs_cmd;
    char *mcs_dest;
} SDBQOSMCS;

typedef struct SDBQOSStoreEvent {
    SDBQOSEvent se_event;
    SDBQOSMCS *se_data;
    unsigned long long se_t;
    unsigned int se_agent;
    struct SDBQOSStoreEvent *se_next;
} SDBQOSStoreEvent;

typedef struct SDBQOSStoreEventIndex {
    SDBQOSStoreEvent *ei_first;
    SDBQOSStoreEvent *ei_last;
    pthread_mutex_t ei_lock;
} SDBQOSStoreEventIndex;

SDBQOSStoreEventIndex qos_table[SDB_QOS_TABLE_MAX];

static int table_hash(unsigned int agent)
{
    /* There should not be many agents, so this is a valid hash function */
    return agent % SDB_QOS_TABLE_MAX;
}

static void qos_mcs_free(SDBQOSMCS *qos_pkt)
{
    if(qos_pkt != NULL) {
        if (qos_pkt->mcs_dest != NULL) {
            free(qos_pkt->mcs_dest);
        }

        free(qos_pkt);
    }
}

#if SDB_QOS_ACTIVE
static SDBQOSMCS *mcs_packet_to_qos_mcs(const MCSPacket *pkt)
{
    SDBQOSMCS *qos_pkt;

    if(pkt == NULL) {
        return NULL;
    } else {
        /* Discard the MCS packet data */
        qos_pkt = malloc(sizeof(*qos_pkt));
        qos_pkt->mcs_id = pkt->id;
        qos_pkt->mcs_type = pkt->type;
        qos_pkt->mcs_cmd = pkt->cmd;
        if(pkt->dest != NULL) {
            qos_pkt->mcs_dest = abs_malloccpy(pkt->dest, strlen(pkt->dest) + 1);
        } else {
            qos_pkt->mcs_dest = NULL;
        }

        return qos_pkt;
    }
}

static void register_event(SDBQOSEvent event, const MCSPacket *pkt)
{
    SDBQOSStoreEvent *store_event;
    SDBModule *mod = get_info();
    struct timeval tv;
    int hash;

    if (mod->qos_enabled) {
        gettimeofday(&tv, NULL);

        store_event = malloc(sizeof(*store_event));

        store_event->se_event = event;
        store_event->se_data = mcs_packet_to_qos_mcs(pkt);
        store_event->se_t = ((unsigned long long)tv.tv_sec * US_TO_SEC)
                                                + tv.tv_usec;
        store_event->se_agent = mod->id;
        store_event->se_next = NULL;

        hash = table_hash(mod->id);
        pthread_mutex_lock(&qos_table[hash].ei_lock);
        if (qos_table[hash].ei_last != NULL) {
            qos_table[hash].ei_last->se_next = store_event;
            qos_table[hash].ei_last = store_event;
        } else {
            qos_table[hash].ei_first = store_event;
            qos_table[hash].ei_last = store_event;
        }
        pthread_mutex_unlock(&qos_table[hash].ei_lock);
    }
}
#else
static void register_event(SDBQOSEvent event, const MCSPacket *pkt)
{
    (void)event;
    (void)pkt;
}
#endif

void sdb_qos_init(void)
{
    int i;

    if (SDB_QOS_ACTIVE) {
        memset(qos_table, 0, SDB_QOS_TABLE_MAX * sizeof(*qos_table));
        for (i = 0; i < SDB_QOS_TABLE_MAX; ++i) {
            pthread_mutex_init(&qos_table[i].ei_lock, NULL);
        }
    }
}

void sdb_qos_start(void)
{
    SDBModule *mod = get_info();
    mod->qos_enabled = true;
}

void sdb_qos_stop(void)
{
    SDBModule *mod = get_info();
    mod->qos_enabled = false;
}

int sdb_qos_dump_module(const MCSPacket *from, MCSPacket **out)
{
    SDBModule *mod = get_info();
    SDBQOSStoreEvent *event, *cur_event, *old_event;
    SDBQOSMCS *qos_pkt;
    char *output;
    char *dest;
    char empty_str[] = "";
    int output_pos, output_size;
    int hash;

    if (!mod->qos_enabled) {
        return -1;
    }

    hash = table_hash(mod->id);
    pthread_mutex_lock(&qos_table[hash].ei_lock);
    event = qos_table[hash].ei_first;
    old_event = NULL;
    output_pos = 0;
    output_size = 500;
    output = malloc(output_size * sizeof(*output));

    while(event != NULL) {
        if(event->se_agent == mod->id) {
            cur_event = event;

            qos_pkt = cur_event->se_data;
            if (qos_pkt->mcs_dest == NULL) {
                dest = empty_str;
            } else {
                dest = qos_pkt->mcs_dest;
            }

            switch (cur_event->se_event) {
                case SDB_QOS_PKT_IN:
                    output_pos += sprintf(output + output_pos,
                            "in|%u|%u|%hu|%s|%llu\n", qos_pkt->mcs_id,
                            qos_pkt->mcs_type, qos_pkt->mcs_cmd, dest,
                            event->se_t);
                    break;
                case SDB_QOS_PKT_OUT:
                    output_pos += sprintf(output + output_pos,
                            "out|%u|%u|%hu|%s|%llu\n", qos_pkt->mcs_id,
                            qos_pkt->mcs_type, qos_pkt->mcs_cmd, dest,
                            event->se_t);
                    break;
                case SDB_QOS_PKT_SCRAP:
                    output_pos += sprintf(output + output_pos,
                            "scrap|%u|%u|%hu|%s|%llu\n", qos_pkt->mcs_id,
                            qos_pkt->mcs_type, qos_pkt->mcs_cmd, dest,
                            event->se_t);
                    break;
                case SDB_QOS_PKT_READY:
                    output_pos += sprintf(output + output_pos,
                            "ready|%u|%u|%hu|%s|%llu\n", qos_pkt->mcs_id,
                            qos_pkt->mcs_type, qos_pkt->mcs_cmd, dest,
                            event->se_t);
                    break;
            }

            if (output_pos > output_size - 50) {
                output_size *= 1.5;
                output = realloc(output, output_size);
            }

            event = cur_event->se_next;
            if(old_event == NULL) {
                qos_table[hash].ei_first = event;
            } else {
                old_event->se_next = event;
            }

            qos_mcs_free(qos_pkt);
            free(cur_event);
        } else {
            old_event = event;
            event = event->se_next;
        }
    }

    qos_table[hash].ei_last = old_event;
    pthread_mutex_unlock(&qos_table[hash].ei_lock);

    *out = mcs_ok_packet_data(from, output, output_pos + 1);
    free(output);
    return 0;
}

void sdb_qos_register_packet_in(const MCSPacket *pkt)
{
    register_event(SDB_QOS_PKT_IN, pkt);
}

void sdb_qos_register_packet_out(const MCSPacket *pkt)
{
    register_event(SDB_QOS_PKT_OUT, pkt);
}

void sdb_qos_register_packet_ready(const MCSPacket *pkt)
{
    register_event(SDB_QOS_PKT_READY, pkt);
}

void sdb_qos_register_packet_scrap(const MCSPacket *pkt)
{
    register_event(SDB_QOS_PKT_SCRAP, pkt);
}
