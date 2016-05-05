#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "sdb_private.h"

int fd_usb;
int id_process = 0;

SDBQueue sdb_usb_queue_send;
SDBQueue sdb_usb_queue_receive;

static unsigned char *mcs_to_usb(const struct SDBPacket *sdb_packet,
                                int *usb_packet_size)
{
    unsigned char *usb_packet;
    MCSPacket *packet = sdb_packet->pkt;
    MCSPacket *response;

    /* Convert a SDB packet to a USB packet */
    *usb_packet_size = packet->data_size + 6;
    if(packet->type != MCS_TYPE_PAYLOAD ||
        packet->cmd >= mcs_command_list_size[MCS_TYPE_PAYLOAD]) {
        printf_dbg("Packet for USB with incorrect type\n");
        goto error;
    }

    if(*usb_packet_size > SDB_USB_DATA_SIZE) {
        printf_dbg("Packet for USB too big\n");
        goto error;
    }

    if(packet->id > 0x7F) {
        printf_dbg("Packet ID not valid for USB\n");
        goto error;
    }

    if(packet->nargs > 2) {
        printf_dbg("Too many arguments\n");
        goto error;
    }

    usb_packet =  malloc(*usb_packet_size);

    usb_packet[0] = ((mcs_command_payload_list[packet->cmd].command << 5) & 0xE0) +
                    ((mcs_command_payload_list[packet->cmd].parameters << 1) & 0x1E) +
                    (1 & 0x1);

    if(packet->nargs == 0) {
        usb_packet[1] = 1;
        usb_packet[2] = 1;
    } else if(packet->nargs == 1) {
        usb_packet[1] = (packet->args[0] << 1) + 1;
        usb_packet[2] = 1;
    } else if(packet->nargs == 2) {
        usb_packet[1] = (packet->args[0] << 1) + 1;
        usb_packet[2] = (packet->args[1] << 1) + 1;
    }

    usb_packet[3] = (unsigned char)((packet->data_size >> 6) & 0xFE) + 1;
    usb_packet[4] = (unsigned char)(packet->data_size << 1) + 1;

    if (packet->data_size > 0) {
        memcpy(&(usb_packet[5]), packet->data, packet->data_size);
    }
    usb_packet[5 + packet->data_size] = packet->id << 1;

    return usb_packet;

error:
    response = mcs_err_packet(packet, EBADFORMAT);
    sdb_module_write_mcs_packet(response, sdb_packet->id_process);
    mcs_free(response);
    return NULL;
}

static MCSPacket *usb_to_mcs(unsigned char *usb_packet, int usb_packet_size)
{
    int data_size;
    unsigned char response_type;
    unsigned char pkt_id;
    unsigned char *data = NULL;
    MCSPacket *response;

    /* Convert a USB packet to a MCS packet */
    if(usb_packet_size < 6) {
        return NULL;
    }

    response_type = usb_packet[0] >> 1;
    data_size = ((usb_packet[3] & (~1)) << 7) | (usb_packet[4] >> 1);
    if (data_size > 0) {
        if (usb_packet_size == data_size + 6) {
            data = malloc(data_size);
            memcpy(data, &(usb_packet[5]), data_size);
        } else {
            printf_dbg("Real and teoric sizes do not match\n");
            return NULL;
        }
    }

    pkt_id = usb_packet[5 + data_size] >> 1;

    switch(response_type) {
        case SDB_USB_OK:
            response = mcs_ok_packet_id(pkt_id);
            break;
        case SDB_USB_OK_DATA:
            response = mcs_ok_packet_data_id(pkt_id, data, data_size);
            break;
        case SDB_USB_ERROR:
            response = mcs_err_packet_id(pkt_id, EHWFAULT);
            break;
        default:
            response = mcs_err_packet_id(pkt_id, EHWFAULT);
            break;
    }

    return response;
}

void *sdb_usb_thread_read(void *arg)
{
    SDBPacket *pkt_origin;
    struct MCSPacket *response;
    unsigned char response_usb[SDB_USB_DATA_SIZE];
    int size;

    (void)arg;

    while(1) {
        size = read(fd_usb, response_usb, SDB_USB_DATA_SIZE);
        response = usb_to_mcs(response_usb, size);
        if(response != NULL) {
            printf_dbg("Received Arduino packet\n");
            pkt_origin = sdb_queue_get(&sdb_usb_queue_receive, response);
            if (pkt_origin != NULL) {
                sdb_module_write_mcs_packet(response, pkt_origin->id_process);
            }
        } else {
            printf_dbg("Invalid packet arrived from USB\n");
        }
    }
}

void *sdb_usb_thread_write(void *arg)
{
    int usb_packet_size;
    struct SDBPacket *sdb_packet;
    unsigned char *usb_packet;

    (void)arg;

    while(1) {
        sdb_packet = sdb_queue_pop_block(&sdb_usb_queue_send);
        usb_packet = mcs_to_usb(sdb_packet, &usb_packet_size);
        if(usb_packet) {
            printf_dbg("Sending Arduino Packet\n");
            sdb_queue_push(&sdb_usb_queue_receive, sdb_packet);
            write(fd_usb, usb_packet, usb_packet_size);
            free(usb_packet);
        } else {
            printf_dbg("Invalid packet arrived to SDB USB\n");
        }
    }
}

int sdb_usb_init(void)
{
    pthread_t t_read, t_write;

    /* Initialize the usb queue */
    sdb_queue_init(&sdb_usb_queue_send);
    sdb_queue_init(&sdb_usb_queue_receive);

    /* Get file descriptor of the USB device */
    fd_usb = open(SDB_USB_DEVICE, O_RDWR);
    if(fd_usb < 0) {
        printf_dbg("Could not open USB\n");
        return EOPEN;
    }

    /* Start usb read thread */
    if(pthread_create(&t_read, NULL, sdb_usb_thread_read, NULL) < 0) {
        printf_dbg("Could not create thread\n");
        return EUNDEF;
    }

    /* Start usb write thread */
    if(pthread_create(&t_write, NULL, sdb_usb_thread_write, NULL) < 0) {
        printf_dbg("Could not create thread\n");
        return EUNDEF;
    }

    return 0;
}
