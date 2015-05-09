#include "libraries.h"

char name[5];

int connectSDB(int id)
{
    int fd;
    struct sockaddr_in addr;
    unsigned int origin_id;
    MCSPacket *pkt;
    char buffer[100];

    char welcome[] = "app0:app";

    sprintf(name, "app%d", id);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SDB_SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* SDB connection */
    sprintf(buffer, "%s: SDB Connection\n", name);
    printf_dbg("Setting connection for %s\n", name);

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        printf_dbg("Could not create the socket\n");
        goto error;
    }

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf_dbg("Could not connect to the socket\n");
        goto error;
    }


    /* Handshaking */
    sprintf(buffer, "%s: Handshake\n", name);
    pkt = mcs_create_packet(MCS_MESSAGE_SDB_HANDSHAKE, 0, NULL,
                    strlen(welcome) + 1, (unsigned char *)welcome);

    if(pkt == NULL) {
        printf_dbg("Packet generated is NULL\n");
        goto error;
    }

    origin_id = pkt->id;

    if(mcs_write_command(pkt, fd) != 0) {
        printf("Could not send handshake packet\n");
        goto error;
    }

    mcs_free(pkt);

    /* Answer to the handshake */
    pkt = mcs_read_command(fd, fd);
    if(pkt == NULL) {
        printf_dbg("Could not read response packet\n");
        goto error;
    } else if(pkt->id != origin_id) {
        printf_dbg("IDs from question and answer are different\n");
    } else if(pkt->type != MCS_TYPE_OK) {
        if(pkt->type == MCS_TYPE_ERR) {
            printf("Wrong response packet. Error %d\n",
                                    mcs_err_code_from_command(pkt));
        } else {
            printf("Wrong reponse packet. Type: %hhd\n", pkt->type);
        }
        goto error;
    }
    return fd;

error:
    return -1;
}

MCSPacket *sendSyncSDB(int fd, MCSPacket *pkt)
{

    char buffer[100];
    unsigned int origin_id;

    sprintf(buffer, "%s: Test packet\n", name);

    if(pkt == NULL) {
        printf_dbg("Packet generated is NULL\n");
        goto error;
    }

    origin_id = pkt->id;

    LOGD("Sending...\n");

    if(mcs_write_command(pkt, fd) != 0) {
        printf_dbg("Error sending TEST packet\n");
        goto error;
    }
    pkt = mcs_read_command(fd, fd);
    if(pkt == NULL) {
        printf_dbg("Could not read response packet\n");
        goto error;
    } else if(pkt->id != origin_id) {
        printf_dbg("IDs from question and answer are different\n");
    } else if(pkt->type != MCS_TYPE_OK) {
        if(pkt->type == MCS_TYPE_ERR) {
            printf_dbg("Wrong response packet. Error %d\n",
                                    mcs_err_code_from_command(pkt));
        } else {
            printf("Wrong reponse packet. Type: %hhd\n", pkt->type);
        }
        goto error;
    }
    LOGD("Receiving...\n");

    MCSPacket *received = abs_malloccpy(pkt, sizeof(*pkt));

    if(received->dest != NULL) {
        ((MCSPacket *)(received))->dest = abs_malloccpy(received->dest, strlen(received->dest));
    }

    if(pkt->nargs != 0) {
        ((MCSPacket *)(received))->args = abs_malloccpy(received->args, received->nargs);
    }

    if(received->data_size != 0) {
        ((MCSPacket *)(received))->data = abs_malloccpy(received->data, received->data_size);
    }

    return received;

 error:
    return NULL;
}