#include "sdb_usb.h"

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

byte[] sdbToUsb(MCSPacket packet)
{
	if(packet.type == MCS_TYPE_PAYLOAD) {

		MCSCommandOptionsPayload = packet.cmd

		byte packet[] = {(byte)packet.command,(byte)10}

		return packet;

	} else {
		return NULL;
	}
}



void* usb_thread(void *arg)
{
	char *packet;

	int fd = open(THE_DEVICE, O_RDWR);

	MCSPacket packet;

	while(packet = usb_queue_pop()){
		char[] buf = sdbToUsb(packet);
		write(fd,&buffer,sizeof(buffer));	
		read(fd,&response);
		pthread_cond_signal(&cond);	
	}
}


