#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <abs.h>
#include <mcs.h>
#include "sdb_private.h"

int main()
{
    pthread_t director_id;
    pthread_t observer_id;
    int ret, sig;
    sigset_t set;

    printf_dbg("Welcome to the (NEW) System Data Bus\n");
    printf_dbg("Smaller, but more powerful!\n");

    /* Block all signals to avoid messing up */
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGFPE);
    sigaddset(&set, SIGSEGV);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    /* Initialize components */
    /*if (sdb_usb_init() != 0) {
        goto error;
    }*/

    sdb_qos_init();

    /* Start director thread */
    if(pthread_create(&director_id, NULL, sdb_director_thread, NULL) < 0) {
        printf_dbg("Error creating thread\n");
        goto error;
    }

    /* Start observer thread */
    if(pthread_create(&observer_id, NULL, sdb_observer_thread, NULL) < 0) {
        printf_dbg("Error creating thread\n");
        goto error;
    }

    while(1) {
        ret = sigwait(&set, &sig);
        if(ret == 0) {
            printf_dbg("Received signal %d\n", sig);
            pthread_cancel(director_id);
            pthread_join(director_id, NULL);
            pthread_cancel(observer_id);
            pthread_join(observer_id, NULL);
            sdb_module_cancel_all();
            exit(0);
        }
    }

error:
    exit(1);
}
