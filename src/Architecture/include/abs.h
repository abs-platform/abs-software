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

#endif

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
