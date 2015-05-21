#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#include <abs.h>

void *abs_malloc0(size_t size)
{
    void *ptr;

    ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void *abs_malloccpy(const void *src, size_t size)
{
    void *ptr;

    ptr = malloc(size);
    memcpy(ptr, src, size);
    return ptr;
}

int abs_write(int fd, char *buffer, int buf_size)
{
    int size;
    
	size = write(fd, buffer, buf_size);
	if(size < 0) {
	    return EUNDEF;
    }
	    
	return size;
}

int abs_read(int fd, char *buffer, int buf_size, int timeout_us)
{
    int r_op;
    fd_set fd_set_select;
    struct timeval max_time;
    struct timeval end;
    int bytes_read;
    int diff_time;

    /* Prepare counters */
    gettimeofday(&end, NULL);
    end.tv_sec += (timeout_us / US_TO_SEC);
    end.tv_usec += (timeout_us % US_TO_SEC);

    if(end.tv_usec > US_TO_SEC) {
        end.tv_sec += 1;
    }
    
    max_time.tv_sec = (timeout_us / US_TO_SEC);
    max_time.tv_usec = (timeout_us % US_TO_SEC);

    bytes_read = 0; 
    while(bytes_read < buf_size) {
        FD_ZERO(&fd_set_select);
        FD_SET(fd, &fd_set_select);

        if(select(FD_SETSIZE, &fd_set_select, NULL, NULL, &max_time) <= 0) {
            return ETIMEOUT;
        }

        r_op = read(fd, buffer, 1);
        buffer += 1;
        bytes_read += 1;
        if(r_op != 1) {
            return EUNDEF;
        }

        /* Calculate and set new timeout */
        gettimeofday(&max_time, NULL);
        diff_time = ((end.tv_sec * US_TO_SEC) + end.tv_usec) - 
                    ((max_time.tv_sec * US_TO_SEC) + max_time.tv_usec);

        max_time.tv_sec  = diff_time / US_TO_SEC;
        max_time.tv_usec = diff_time % US_TO_SEC;
    }

    return bytes_read;
}
