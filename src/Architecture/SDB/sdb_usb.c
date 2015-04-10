#include "sdb_usb.h"

static char *sdb_to_usb(MCSPacket *packet, int *packet_size)
{
    char usb_packet[MAX_SIZE_USB_PACKET];
    MCSCommandOptionsPayload payload_package;

    if(packet->type == MCS_TYPE_PAYLOAD) {
        payload_package = option_payload[packet->cmd];
        packet_size = 5 + payload_package.data_size;

        usb_packet[0] = (payload_package.command << 5) & 0xE0 + (payload_package.parameters << 1) & 0x1E + 1;
        usb_packet[1] = (payload_package.cmd_arg1 << 1) + 1
        usb_packet[2] = (payload_package.cmd_arg2 << 1) + 1
        usb_packet[3] = (payload_package.data_size >> 7) + 1;
        usb_packet[4] = (payload_package.data_size << 1) + 1; 
        for(i=0; i < payload_package.data_size; i++) {
            usb_packet[5 + i] = packet.data[i];  
        }
        usb_packet[6 + i] = 10 << 1;
        
        return usb_packet;
    } else {
        return NULL;
    }
}

void *usb_thread(void *arg)
{
    char *buffer;
    QueueElement *element;
    MCSPacket *response;
    int data_size, packet_size, response_type;
    char response_usb[MAX_SIZE_USB_PACKET];
    int fd = open(SDB_USB_DEVICE, O_RDWR);

    while(element = usb_queue_pop()) {
        buffer = sdb_to_usb(element->data, &packet_size);
        write(fd, buffer, packet_size);
        read(fd, response_usb, MAX_SIZE_USB_PACKET);
        response_type = (response_usb[0] << 3) & 0x03;
        switch(response_type) {
            case OK:
                response = mcs_ok_packet();
                break;
            case OK_DATA:
                data_size = (int)(result[0] << 1);
                response = mcs_ok_packet_data(&response_usb[2], data_size);
                break;
            case ERROR:
                response = mcs_err_packet(EHWFAULT);
                break;
        }
        sdb_module_write_mcs_packet(response, element->id_process, SDB_USB_ID);
        free(buffer);
    }
}