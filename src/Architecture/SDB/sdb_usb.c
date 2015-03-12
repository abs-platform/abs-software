#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "usb_queue.h"

/*
enum MCSType {
    MCS_TYPE_MESSAGE    = 0,
    MCS_TYPE_STATE      = 1,
    MCS_TYPE_PAYLOAD    = 2,
    MCS_TYPE_OK         = 253,
    MCS_TYPE_OK_DATA    = 254,
    MCS_TYPE_ERR        = 255,
};

// Datatype for the packets that can be sent to and from the SDB 
typedef struct MCSPacket {
    enum MCSType type;
    unsigned short cmd;
    unsigned short nargs;
    unsigned char *args;
    unsigned short data_size;
    unsigned char *data;
} MCSPacket;

*/

byte[] mcsConvert(MCSPacket packet)
{
	if(packet.type == MCS_TYPE_PAYLOAD) {

		MCSCommandOptionsPayload = packet.cmd

		byte packet[] = {(byte)packet.command,(byte)10}

		return packet;

	} else {
		return NULL;
	}
}


void sdb_usb()
{
	byte[] packet;

	MCSPacket packet;

	while(1){
		while((packet=usb_queue_pop())==NULL);
	}
}


