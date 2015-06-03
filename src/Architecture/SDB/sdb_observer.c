#define _GNU_SOURCE
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include "sdb_private.h"

SDBModule sdb_module[SDB_MODULE_MAX];
unsigned int sdb_module_last;
pthread_mutex_t sdb_module_lock;
int sdb_observer_fd = -1;

static void sdb_observer_clean(void *arg)
{
    int *pipe = (int *)arg;
    if(pipe[0] != -1) {
        close(pipe[0]);
    }
    if(pipe[1] != -1) {
        close(pipe[1]);
    }
}

void sdb_observer_wake_up(void)
{
    char buf = 'w';
    if(sdb_observer_fd != -1) {
        write(sdb_observer_fd, &buf, 1);
    }
}

void *sdb_observer_thread()
{
    unsigned int last;
    unsigned int i;
    int pipefd[2];
    char buf;
    fd_set fifo_set;

    pipefd[0] = -1;
    pipefd[1] = -1;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(sdb_observer_clean, pipe);

    pipe(pipefd);
    sdb_observer_fd = pipefd[1];

    /* Do not do anything until somebody wakes you up */
    read(pipefd[0], &buf, 1);

    while(1) {
        pthread_mutex_lock(&sdb_module_lock);
        last = sdb_module_last;
        pthread_mutex_unlock(&sdb_module_lock);

        FD_ZERO(&fifo_set);

        FD_SET(pipefd[0], &fifo_set);

        for(i = 0; i < last; ++i) {
            FD_SET(sdb_module[i].rfd, &fifo_set);
        }

        if(select(FD_SETSIZE, &fifo_set, NULL, NULL, NULL) < 0) {
            printf_dbg("Could not block\n");
            continue;
        }

        if(FD_ISSET(pipefd[0], &fifo_set) == 1) {
            read(pipefd[0], &buf, 1);
            continue;
        }

        for(i = 0; i < last; ++i) {
            if(FD_ISSET(sdb_module[i].rfd, &fifo_set) == 1) {
                if(pthread_mutex_trylock(&sdb_module[i].lock) == 0) {
                    printf_dbg("%s has data\n", sdb_module[i].name);
                    sdb_module[i].data_socket = true;
                    pthread_cond_broadcast(&sdb_module[i].cond_var);
                    pthread_mutex_unlock(&sdb_module[i].lock);
                    /* Let other threads run, so they can check their data */
                    pthread_yield();
                }
            }
        }
    }

    /* Should never arrive here! */
    pthread_cleanup_pop(1);
}
