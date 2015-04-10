#include <sys/cdefs.h>

#include <sys/param.h>
#include <sys/stat.h>

#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <pthread.h>

#define MAX_BUF 1024

#define THE_DEVICE "/sys/class/timed_output/vibrator/enable"

int s2c, c2s; int fds;

char *buf;

static int sendit(int timeout_ms)
{
    int nwr, ret, fd, i=0;
    char value[20];

    fd = open(THE_DEVICE, O_RDWR);
    if(fd < 0)
        return errno;

    nwr = sprintf(value, "%d\n", timeout_ms);
    ret = write(fd, value, nwr);

    close(fd);

    return (ret == nwr) ? 0 : -1;
}

void *inc_x(void *x_void_ptr)
{   
    /* digitalWrite(2,HIGH) */
    char packet1[] = "\x2D\x05\x03\x02";
    /* digitalWrite(2,LOW) */
    char packet2[] = "\x29\x05\x01\x02";

    int fc;
    char * myfifo = "/data/test/fifo/myfifo";
    char buf[MAX_BUF];

    int i=0;
    while(1){

        if(i==0){
                    write(fds,packet1,4);
        sleep(2);

            i=1;
        } else {
        write(fds,packet2,4);
        sleep(2);
            i=0;
        }
    }
}

int main()
{
    static size_t bsize;
    static char *buffer = NULL;

    int fd = open("/dev/usb_accessory", O_RDWR);

    fds = fd;

    pthread_t read_thread;

    pthread_create(&read_thread, NULL, inc_x, NULL);
        
    if (buffer == NULL) {   
        bsize=1024;
        if ((buffer = malloc(bsize)) == NULL)
            err(1, "buffer");
    }
    int count = 0;
    while(1){
        read(fd, buffer, bsize);
        printf("hi %d\n",count);
        if(buffer[1]=='\x05')
            sendit(200);
    }
}