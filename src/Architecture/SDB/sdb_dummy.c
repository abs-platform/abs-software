#include "sdb_dummy.h"

void* push(void *arg)
{
    while(1){
        sleep(1);
        usb_queue_push("A1", 1); 
        usb_queue_push("B1", 2); 
        usb_queue_push("A2", 1); 
        usb_queue_push("A3", 1); 
        sleep(1);
        usb_queue_push("C1", 3); 
        usb_queue_push("C2", 3); 
        usb_queue_push("B2", 2); 
    }
}

void* pop(void *arg)
{
    int p; char *c;

    while(1){
        sleep(2);
        c = usb_queue_pop(&p);
        printf("------\n");
        printf("%d: %s\n", p, c);

    }
    
}

int main(int argc , char *argv[])
{
    pthread_t tid[2];

    usb_queue_init();

    pthread_create(&(tid[0]), NULL, &push, NULL);

    pthread_create(&(tid[1]), NULL, &pop, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);	
}


