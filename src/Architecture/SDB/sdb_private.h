#ifndef __SDB_PRIVATE_H
#define __SDB_PRIVATE_H

#include <stdbool.h>
#include <pthread.h>
#include <sdb.h>
#include <mcs.h>

#define SDB_MODULE_DATA_SIZE    1024

#define SDB_MODULE_MAX          20
#define SDB_USB_ID              (SDB_MODULE_MAX + 1)

typedef struct SDBModulePacket {
    unsigned int id_origin;
    MCSPacket *pkt;
} SDBModulePacket;

typedef struct SDBModuleQueue {
    SDBModulePacket *pkt;
    struct SDBModuleQueue *next;
} SDBModuleQueue;

typedef struct SDBModule {
    pthread_t thread_id;
    int rfd; /* The SDB reads from this File Descriptor */
    int wfd; /* The SDB writes to this File Descriptor */
    const char *name;
    SDBGroup group;

    pthread_mutex_t lock;
    pthread_cond_t cond_var;

    void *data;
    bool data_valid;

    bool data_socket;

    SDBModuleQueue *queue_first;
    SDBModuleQueue *queue_last;
} SDBModule;

extern SDBModule sdb_module[SDB_MODULE_MAX];
extern unsigned int sdb_module_last;
extern pthread_mutex_t sdb_module_lock;
extern int sdb_observer_fd;

/* SDB module section */
void sdb_module_init(int rfd, int wfd);
int sdb_module_write_mcs_packet(const MCSPacket *pkt, unsigned int to,
                                                        unsigned int from);
MCSPacket *sdb_module_read_mcs_packet(unsigned int my_id);
void sdb_module_cancel_all(void);
void *sdb_module_thread(void *arg);

/* SDB director section */
void *sdb_director_thread();

/* SDB observer section */
void *sdb_observer_thread();
void sdb_observer_wake_up(void);
#endif
