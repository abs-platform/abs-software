#include "sdb_usb.h"

int stop_read = 0, stop_write = 0;

int id_process = 0;

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
        return usb_packet;
    } else {
        return NULL;
    }
}

void* eread(void* pv)
{
    int fd;
    char response_usb[MAX_SIZE_USB_PACKET];

    fd = *((int*)pv);
    
    while(!stop_read) {
        if(read(fd, response_usb, MAX_SIZE_USB_PACKET) > 0) {
            printf_dbg(("Receiving Arduino Packet...\n");
            
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
            sdb_module_write_mcs_packet(response, id_process, SDB_USB_ID);
        }
    }
}

void* ewrite(void* pv)
{
    int fd;
    int usb_packet_size;
    MCSPacket *mcs_packet;
    unsigned char *usb_packet;

    fd = *((int*)pv);

    while(!stop_write) {
        mcs_packet = usb_queue_pop(&id_process);
        printf_dbg(("Sending Arduino Packet...\n");
        usb_packet = sdb_to_usb(mcs_packet, &usb_packet_size);
        write(fd, usb_packet, usb_packet_size);
    }
}

void sdb_usb_init()
{
    int fd;
    pthread_t ptread, ptwrite;
    usb_queue_init();
    /* Get file descriptor of the USB device */
    fd = open(SDB_USB_DEVICE, O_RDWR);
    if(fd < 0) {
        return -1001;
    }
    /* Start usb read thread */
    if(pthread_create(&ptread, NULL, eread, &fd) < 0) {
        return -1002;
    }
    /* Start usb write thread */
    if(pthread_create(&ptwrite, NULL, ewrite, &fd) < 0) {
        return -1002;
    }
    return 0;
}
