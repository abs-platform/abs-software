#include <pthread.h>
#include <stdio.h>
#include "sdb_private.h"

int main()
{
    pthread_t director_id;
    pthread_t observer_id;

    printf_dbg("Welcome to the (NEW) System Data Bus\n");
    printf_dbg("Smaller, but more powerful!\n");

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

    while(1) {}

error:
    exit(1);
}
