#include "sdb_usb.h"

static unsigned char *sdb_to_usb(MCSPacket *packet, int *usb_packet_size)
{
    int i = 0;
    *usb_packet_size =  5 + packet->data_size;
    unsigned char *usb_packet =  malloc(MAX_SIZE_USB_PACKET);
    if(packet->type == MCS_TYPE_PAYLOAD) {
        usb_packet[0] = ((mcs_command_payload_list[packet->cmd].command << 5) & 0xE0) + 
            ((mcs_command_payload_list[packet->cmd].parameters << 1) & 0x1E) + (1 & 0x1);
        usb_packet[1] = (packet->args[0] << 1) + 1;
        usb_packet[2] = (packet->args[1] << 1) + 1;
        usb_packet[3] = (packet->data_size >> 7) + 1;
        usb_packet[4] = (packet->data_size << 1) + 1; 
        for(i=0; i < packet->data_size; i++) {
            usb_packet[5 + i] = packet->data[i];  
        }
        usb_packet[6 + i] = (10) << 1;   
        return &usb_packet[0];
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