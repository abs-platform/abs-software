#ifndef __SDB_H
#define __SDB_H

#include <stdbool.h>
#include <mcs.h>

#define SDB_SOCK_PORT 31415
#define SDB_MODULE_NAME_SIZE 10

typedef struct SDBPendingPacket {
    unsigned int pp_id;
    MCSPacket *pp_pkt;
    bool pp_valid;
    pthread_mutex_t pp_lock;
    pthread_cond_t pp_cond;
} SDBPendingPacket;

typedef void (*SDBCallback)(MCSPacket *, MCSPacket **);

void sdb_init(void) __attribute__((constructor));
void sdb_uninit(void) __attribute__((destructor));

int sdb_connect(const char *name, SDBGroup group);
int sdb_disconnect(void);
int sdb_register_callback(MCSCommand cmd, SDBCallback callback);
int sdb_unregister_callback(MCSCommand cmd);
int sdb_send_sync(MCSPacket *pkt_in, MCSPacket **pkt_out);
int sdb_send_sync_and_free(MCSPacket *pkt_in, MCSPacket **pkt_out);
int sdb_send_async(MCSPacket *pkt_in, SDBPendingPacket **pkt_out);
int sdb_send_async_and_free(MCSPacket *pkt_in, SDBPendingPacket **pkt_out);
int sdb_wait_async(SDBPendingPacket *pkt);
int sdb_check_async(SDBPendingPacket *pkt);
void sdb_pending_packet_free(SDBPendingPacket *pkt);

#endif
