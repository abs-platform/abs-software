#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/poll.h>
#include <sys/ioctl.h>

#define SDB_USB_DEVICE "/dev/usb_accessory"

int main(int argc, char const *argv[])
{
    int fd = open(SDB_USB_DEVICE, O_RDWR);
 
    /* Examples of basic I/O and event packets */

    /*
     * Basic I/O
     *
     *	0	0	1	0	1	1	0	1 <- read digital pin (57) x2D
     *	0	0	0	0	0	1	0	1 <- digital pin nº 2  (5) x05
     *  0   0   0   0   0   0   1   1 <- digital pin nº 2  (5) x05
     *	0	0	0	0	0	0	1	0 <- end & packet = 1  (2) x02
     *
     */

    /*
     * Basic Event
     *
     *	0	1	1	1	0	0	0	1 <- set event prescaler 2	(101)	0x65
     *	0	0	0	0	0	0	1	1 <- act 1: digitalWrite	  (3) 	0x03
     *	0	0	0	1	0	0	0	1 <- opt 1: pin  number		  (8)	0x11
     *	0	0	0	0	0	1	0	0 <- end & packet = 2  		  (2) 	0x04
     *
     */

    static size_t bsize=100;
    static char *buffer = NULL;

    if (buffer == NULL) {   
        bsize=1024;
        if ((buffer = malloc(bsize)) == NULL)
            printf("Error with buffer\n");
    }

    size_t offset = 0; 
    size_t bytes_read; 
    int i; 

    /* digitalWrite(2,HIGH) */
    char packet1[] = "\x2D\x05\x03\x02";
    /* digitalWrite(2,LOW) */
    char packet2[] = "\x29\x05\x01\x02";
    /* Event pre=2 toggle(8) */
    char packet3[] = "\x65\x03\x00\x00\x09\x2D\x05\x03\x02\x04";
    /* Event pre=8 toggle(8) */
    char packet4[] = "\x65\x03\x00\x00\x09\x2D\x05\x01\x02\x04";
    /* create an event */
    write(fd, packet3, sizeof(packet3));
    i=read(fd, buffer, bsize);
    printf("result: %d\n",i);
    sleep(5);
    /* create an event */
    write(fd, packet4, sizeof(packet4));	
    i=read(fd, buffer, bsize);
    printf("result: %d\n",i);
    sleep(5);   

    while(1) {
        /* blink a led */
        write(fd, packet1, sizeof(packet1));
        i=read(fd, buffer, bsize);
        printf("result: %d\n",i);
        sleep(1);
        write(fd, packet2, sizeof(packet2));
        i=read(fd, buffer, bsize);
        printf("result: %d\n",i);
        sleep(1);
    }
    return 0;
}

