#include <stdio.h>
#include <sys/select.h>
#include "sdb_private.h"

struct SDBModule sdb_module[SDB_MODULE_MAX];
unsigned int sdb_module_last;
pthread_mutex_t sdb_module_lock;

void *sdb_observer_thread()
{
    unsigned int last;
    unsigned int i;
    fd_set fifo_set;

    while(1) {
        pthread_mutex_lock(&sdb_module_lock);
        last = sdb_module_last;
        pthread_mutex_unlock(&sdb_module_lock);

        FD_ZERO(&fifo_set);

        for(i = 0; i < last; ++i) {
            FD_SET(sdb_module[i].rfd, &fifo_set);
        }

        if(select(FD_SETSIZE, &fifo_set, NULL, NULL, NULL) < 0) {
            printf_dbg("Could not block\n");
            continue;
        }

        for(i = 0; i < last; ++i) {
            if(FD_ISSET(sdb_module[i].rfd, &fifo_set) == 1) {
                pthread_mutex_lock(&sdb_module[i].lock);
                sdb_module[i].data_socket = true;
                pthread_cond_broadcast(&sdb_module[i].cond_var);
                pthread_mutex_unlock(&sdb_module[i].lock);
            }
        }
    }
}
