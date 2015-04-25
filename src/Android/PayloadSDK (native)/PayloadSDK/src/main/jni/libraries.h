#ifndef __LIBRARIES_H__
#define __LIBRARIES_H__

#include <mcs.h>
#include <sdb.h>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <android/log.h>

#define TAG "[ABS]"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

int connectSDB(int id);

int sendSyncSDB(int fd, MCSPacket *pkt);

#endif // __LIBRARIES_H__