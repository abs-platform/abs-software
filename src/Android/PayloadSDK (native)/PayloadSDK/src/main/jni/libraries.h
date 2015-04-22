#ifndef __LIBRARIES_H__
#define __LIBRARIES_H__

#include <mcs.h>
#include <sdb.h>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int connectSDB(int id);

int sendSyncSDB(int fd, MCSPacket *pkt);

#endif // __LIBRARIES_H__