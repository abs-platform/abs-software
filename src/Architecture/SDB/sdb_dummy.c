#include "sdb_dummy.h"

pthread_cond_t cond  = PTHREAD_COND_INITIALIZER;

void* push(void *arg)
{
    while(1){
        usb_queue_push("A1", 1); 
        sleep(1);
    }
}



int main(int argc , char *argv[])
{
    pthread_t tid;

    usb_queue_init();

    pthread_create(&(tid), NULL, &sdb_usb, NULL);

    while(1)
    {
        usb_queue_push("hi",1);
        pthread_cond_wait(&cond);

    }



}


