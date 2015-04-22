#ifndef __ABS_H
#define __ABS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
    #define printf_dbg(fmt, ...) \
        do { \
            fprintf(stderr, "[%s:%s:L.%d] " fmt, \
                    __FILE__, __func__, __LINE__, ## __VA_ARGS__); \
        } while (0)
#else
    #define printf_dbg(fmt, ...) do { } while (0)
#endif

#define enum_string(T, name)    \
    static const struct {       \
        T val;                  \
        const char *str;        \
    } name[] = {

#define MS_TO_SEC   1000
#define US_TO_MS    1000
#define US_TO_SEC   1000000
#define NS_TO_SEC   1000000000

/* Error codes: */
/* Common */
#define     ABS_EUNDEF          -1
#define     ABS_EOPEN           -2
#define     ABS_EMUTEX          -3
#define     ABS_EPROG           -4  /* Programming error (e.g. Segmentation Fault) */
#define     ABS_EWARNING        -5
#define     ABS_ESOCKET         -6
#define     ABS_ECONFIG         -7
#define     ABS_ETIMEOUT        -8
#define     ABS_EWRONGSTATE     -9
#define     ABS_ECONNREFUSED    -10
#define     ABS_EBUSY           -11
#define     ABS_EFILESYS        -12
#define     ABS_EPERM           -13

/* Specific (starting at -100) */
#define     ABS_ECREATERT       -100    /* Unable to create RT task */
#define     ABS_EBATTERY        -101
#define     ABS_ELOWENERGY      -102
#define     ABS_EFORCE          -103    /* Could not force an action */
#define     ABS_EBADMODID       -104
#define     ABS_ETHREAD         -105    /* Could not create / join thread */
#define     ABS_EPOLDIS         -106    /* Point of load is disabled */
#define     ABS_EPOLERR         -107    /* Point of load has errored */
#define     ABS_ECMDFAULT       -108    /* Command not valid */
#define     ABS_ESETUP          -109
#define     ABS_EHWFAULT        -110
#define     ABS_ECHECKSUM       -111
#define     ABS_EDISCONNECT     -112
#define     ABS_EBADFORMAT      -113    /* Packet doesn't have valid format */

/* End of error codes */

void *abs_malloc0(size_t size);
void *abs_malloccpy(const void *src, size_t size);
int abs_write(int fd, char *buffer, int buf_size);
int abs_read(int fd, char *buffer, int buf_size, int timeout);

#endif
