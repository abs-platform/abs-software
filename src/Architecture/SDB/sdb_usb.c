#include "sdb_usb.h"

int fd_usb;
int id_process = 0;

int stop_read = 0, stop_write = 0;

static unsigned char *sdb_to_usb(const struct MCSPacket *packet, int *usb_packet_size)
{
    int i = 0;
    /* Convert a SDB packet to a USB packet */
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

void* eread()
{
    int response_type, data_size;
    const struct MCSPacket *response;
    char response_usb[MAX_SIZE_USB_PACKET];

    while(!stop_read) {
        if(read(fd_usb, response_usb, MAX_SIZE_USB_PACKET) > 0) {
            printf_dbg("Receiving Arduino Packet...\n");
            response_type = (response_usb[0] >> 1) & 0x03;
            switch(response_type) {
                case OK:
                    response = mcs_ok_packet(id_process);
                    break;
                case OK_DATA:
                    data_size = response_usb[3] >> 1;
                    response = mcs_ok_packet_data(id_process, &response_usb[4], data_size);
                    break;
                case ERROR:
                    response = mcs_err_packet(id_process, ABS_EHWFAULT);
                    break;
                default:
                    response = mcs_err_packet(id_process, ABS_EHWFAULT);
                    break;
            }
            sdb_module_write_mcs_packet(response, (unsigned int)id_process);
        }
    }
    return 0;
}

void* ewrite()
{
    int usb_packet_size;
    const struct MCSPacket *mcs_packet;
    unsigned char *usb_packet = malloc(100);

    while(!stop_write) {
        mcs_packet = usb_queue_pop(&id_process);
        printf_dbg("Sending Arduino Packet...\n");
        usb_packet = sdb_to_usb(mcs_packet, &usb_packet_size);
        if(usb_packet) {            
            write(fd_usb, usb_packet, usb_packet_size);
        } else {
            printf_dbg("Bad packet\n");
        }
    }

    return 0;
}

int sdb_usb_init()
{
    pthread_t ptread, ptwrite;
    /* Initialize the usb queue */
    usb_queue_init();
    /* Get file descriptor of the USB device */
    fd_usb = open(SDB_USB_DEVICE, O_RDWR);
    if(fd_usb < 0) {
        return -1001;
    }
    /* Start usb read thread */
    if(pthread_create(&ptread, NULL, eread, NULL) < 0) {
        return -1002;
    }
    /* Start usb write thread */
    if(pthread_create(&ptwrite, NULL, ewrite, NULL) < 0) {
        return -1002;
    }
    return 0;
}
