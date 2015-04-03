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
#define     EUNDEF          -1
#define     EOPEN           -2
#define     EMUTEX          -3
#define     EPROG           -4  /* Programming error (e.g. Segmentation Fault) */
#define     EWARNING        -5
#define     ESOCKET         -6
#define     ECONFIG         -7
#define     ETIMEOUT        -8
#define     EWRONGSTATE     -9
#define     ECONNREFUSED    -10
#define     EBUSY           -11
#define     EFILESYS        -12
#define     EPERM           -13

/* Specific (starting at -100) */
#define     ECREATERT       -100    /* Unable to create RT task */
#define     EBATTERY        -101
#define     ELOWENERGY      -102
#define     EFORCE          -103    /* Could not force an action */
#define     EBADMODID       -104
#define     ETHREAD         -105    /* Could not create / join thread */
#define     EPOLDIS         -106    /* Point of load is disabled */
#define     EPOLERR         -107    /* Point of load has errored */
#define     ECMDFAULT       -108    /* Command not valid */
#define     ESETUP          -109
#define     EHWFAULT        -110
#define     ECHECKSUM       -111
#define     EDISCONNECT     -112
#define     EBADFORMAT      -113    /* Packet doesn't have valid format */

/* End of error codes */

static inline void *abs_malloc0(size_t size)
{
    void *ptr;

    ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

static inline void *abs_malloccpy(const void *src, size_t size)
{
    void *ptr;

    ptr = malloc(size);
    memcpy(ptr, src, size);
    return ptr;
}

#endif
