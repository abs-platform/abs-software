#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mcs.h>
#include <sdb.h>
#include <abs.h>

#define SDB_LIST_MIN 5

typedef struct SDBCMDCallback {
    MCSCommand cmdcb_cmd;
    SDBCallback cmdcb_callback;
} SDBCMDCallback;

typedef struct SDBPacketCallback {
    MCSPacket *pktcb_pkt;
    SDBCallback pktcb_callback;
} SDBPacketCallback;

typedef struct SDBPendingQueueElem {
    SDBPendingPacket queue_pkt;
    struct SDBPendingQueueElem *queue_next;
    struct SDBPendingQueueElem *queue_prev;
} SDBPendingQueueElem;

unsigned int callback_list_size;
unsigned int callback_list_pos;
SDBCMDCallback *callback_list;
pthread_mutex_t callback_list_lock;

SDBPendingQueueElem *pending_list_first = NULL;
pthread_mutex_t pending_list_lock;

pthread_t read_thread_id;
int socket_fd = -1;

static int get_callback_index(MCSCommand cmd)
{
    int i;

    for(i = 0; i < (int)callback_list_pos; ++i) {
        if(callback_list[i].cmdcb_cmd == cmd) {
            return i;
        }
    }

    return -1;
}

static SDBPendingPacket *get_pending_packet_id(unsigned int pkt_id)
{
    SDBPendingQueueElem *queue_elem;

    queue_elem = pending_list_first;
    while(queue_elem != NULL) {
        if(queue_elem->queue_pkt.pp_id == pkt_id) {
            return &queue_elem->queue_pkt;
        }

        queue_elem = queue_elem->queue_next;
    }

    return NULL;
}

static SDBPendingQueueElem *add_pending_id(unsigned int pkt_id)
{
    SDBPendingQueueElem *queue_elem = malloc(sizeof(*queue_elem));

    queue_elem->queue_pkt.pp_id = pkt_id;
    queue_elem->queue_pkt.pp_valid = false;
    pthread_mutex_init(&queue_elem->queue_pkt.pp_lock, NULL);
    pthread_cond_init(&queue_elem->queue_pkt.pp_cond, NULL);

    pthread_mutex_lock(&pending_list_lock);
    if (pending_list_first != NULL) {
        pending_list_first->queue_prev = queue_elem;
    }
    queue_elem->queue_next = pending_list_first;
    queue_elem->queue_prev = NULL;
    pending_list_first = queue_elem;
    pthread_mutex_unlock(&pending_list_lock);

    return queue_elem;
}

static void delete_pending_id(unsigned int pkt_id)
{
    SDBPendingQueueElem *queue_elem;

    pthread_mutex_lock(&pending_list_lock);
    queue_elem = pending_list_first;
    while(queue_elem != NULL) {
        if(queue_elem->queue_pkt.pp_id == pkt_id) {
            if(queue_elem->queue_next != NULL) {
                queue_elem->queue_next->queue_prev = queue_elem->queue_prev;
            }

            if(queue_elem == pending_list_first) {
                pending_list_first = queue_elem->queue_next;
            } else {
                queue_elem->queue_prev->queue_next = queue_elem->queue_next;
            }

            pthread_mutex_destroy(&queue_elem->queue_pkt.pp_lock);
            pthread_cond_destroy(&queue_elem->queue_pkt.pp_cond);
            free(queue_elem);
            break;
        }

        queue_elem = queue_elem->queue_next;
    }
    pthread_mutex_unlock(&pending_list_lock);
}

void *sdb_thread_run_callback(void *args)
{
    SDBPacketCallback *pkt_callback = (SDBPacketCallback *)args;
    MCSPacket *pkt_out;

    pkt_callback->pktcb_callback(pkt_callback->pktcb_pkt, &pkt_out);
    free(pkt_callback);

    mcs_write_command_and_free(pkt_out, socket_fd);

    pthread_exit(NULL);
}

void *sdb_thread_read(void *args)
{
    int index;
    MCSPacket *pkt;
    SDBPacketCallback *pkt_callback;
    SDBPendingPacket *pkt_pending;
    fd_set socket_set;
    pthread_t th_id;

    (void)args;

    while(1) {
        FD_ZERO(&socket_set);
        FD_SET(socket_fd, &socket_set);

        if(select(FD_SETSIZE, &socket_set, NULL, NULL, NULL) < 0) {
            printf_dbg("Could not block\n");
            continue;
        }

        if(FD_ISSET(socket_fd, &socket_set) == 1) {
            pkt = mcs_read_command(socket_fd, socket_fd);

            if(mcs_is_answer_packet(pkt)) {
                /* Is this packet in pending list? */
                pthread_mutex_lock(&pending_list_lock);
                pkt_pending = get_pending_packet_id(pkt->id);
                if(pkt_pending != NULL) {

                    pthread_mutex_lock(&pkt_pending->pp_lock);
                    pkt_pending->pp_pkt = pkt;
                    pkt_pending->pp_valid = true;
                    pthread_cond_broadcast(&pkt_pending->pp_cond);
                    pthread_mutex_unlock(&pkt_pending->pp_lock);
                    pthread_mutex_unlock(&pending_list_lock);
                    continue;
                }
                pthread_mutex_unlock(&pending_list_lock);
            } else {
                /* Is this packet in callback list? */
                pthread_mutex_lock(&callback_list_lock);
                index = get_callback_index(pkt->cmd);
                if(index != -1) {
                    pkt_callback = malloc(sizeof(*pkt_callback));
                    pkt_callback->pktcb_pkt = pkt;
                    pkt_callback->pktcb_callback =
                                    callback_list[index].cmdcb_callback;
                    pthread_create(&th_id, NULL, sdb_thread_run_callback,
                                    pkt_callback);
                    pthread_mutex_unlock(&callback_list_lock);
                    continue;
                }
                pthread_mutex_unlock(&callback_list_lock);
            }

            printf_dbg("Received unexpected packet from SDB\n");
            mcs_write_command_and_free(mcs_err_packet(pkt, ECMDFAULT),
                                        socket_fd);
            mcs_free(pkt);
        }
    }
}

void sdb_init(void)
{
    callback_list_pos = 0;
    callback_list_size = SDB_LIST_MIN;
    callback_list = malloc(sizeof(*callback_list) * callback_list_size);
    pthread_mutex_init(&callback_list_lock, NULL);

    pending_list_first = NULL;
    pthread_mutex_init(&pending_list_lock, NULL);
}

void sdb_uninit(void)
{
    free(callback_list);
    pthread_mutex_destroy(&callback_list_lock);

    pthread_mutex_destroy(&pending_list_lock);
}

int sdb_connect(const char *name, SDBGroup group)
{
    int ret;
    char handshake_str[SDB_MODULE_NAME_SIZE * 2];
    MCSPacket *pkt_in;
    MCSPacket *pkt_out;
    struct sockaddr_in addr;

    if(socket_fd != -1) {
        printf_dbg("Already connected to the SDB\n");
        return 0;
    }

    if(name == NULL) {
        printf_dbg("Name cannot be NULL\n");
        return -1;
    }

    if(strlen(name) > SDB_MODULE_NAME_SIZE) {
        printf_dbg("Name cannot be longer than %d characters\n",
                    SDB_MODULE_NAME_SIZE);
        return -1;
    }

    if(group >= SDB_GROUP_MAX) {
        printf_dbg("Group not valid\n");
        return -1;
    }

    sprintf(handshake_str, "%s:%s", name, sdb_group_conversion[group].str);

    /* Create socket */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SDB_SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf_dbg("Could not create the socket\n");
        return -1;
    }

    if(connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf_dbg("Could not connect to the SDB\n");
        ret = -1;
        goto error_close;
    }

    /* Create reading thread */
    pthread_create(&read_thread_id, NULL, sdb_thread_read, NULL);

    /* Handshake with SDB */
    pkt_in = mcs_create_packet(MCS_MESSAGE_SDB_HANDSHAKE, 0, NULL,
                    strlen(handshake_str) + 1, (unsigned char *)handshake_str);
    ret = sdb_send_sync_and_free(pkt_in, &pkt_out);
    if(ret != 0) {
        printf_dbg("Error sending the handshake\n");
        goto error_close;
    }

    if(pkt_out->type == MCS_TYPE_OK) {
        free(pkt_out);
        return 0;
    } else if(pkt_out->type == MCS_TYPE_ERR) {
        printf_dbg("Error handshaking (%d)\n",
                            mcs_err_code_from_command(pkt_out));
        free(pkt_out);
        ret = -1;
        goto error_close;
    } else {
        printf_dbg("Error handshaking. Unexpected answer\n");
        free(pkt_out);
        ret = -1;
        goto error_close;
    }

error_close:
    close(socket_fd);
    return ret;
}

int sdb_disconnect(void)
{
    close(socket_fd);

    return 0;
}

int sdb_register_callback(MCSCommand cmd, SDBCallback callback)
{
    if(callback == NULL) {
        printf_dbg("Callback cannot be NULL\n");
        return -1;
    }

    pthread_mutex_lock(&callback_list_lock);
    if(get_callback_index(cmd) != -1) {
        printf_dbg("Callback is already registered\n");
        return -1;
    }

    if(callback_list_pos == callback_list_size) {
        callback_list_size *= 1.5;
        callback_list = realloc(callback_list,
                                sizeof(*callback_list) * callback_list_size);
    }

    callback_list[callback_list_pos].cmdcb_cmd = cmd;
    callback_list[callback_list_pos].cmdcb_callback = callback;
    ++callback_list_pos;
    pthread_mutex_unlock(&callback_list_lock);

    return 0;
}

int sdb_unregister_callback(MCSCommand cmd)
{
    int i;
    int index;

    pthread_mutex_lock(&callback_list_lock);
    index = get_callback_index(cmd);

    if(index != -1) {
        for(i = index + 1; i < (int)callback_list_pos; ++i) {
            callback_list[i - 1].cmdcb_cmd = callback_list[i].cmdcb_cmd;
            callback_list[i - 1].cmdcb_callback = callback_list[i].cmdcb_callback;
        }
    }
    pthread_mutex_unlock(&callback_list_lock);

    return 0;
}

int sdb_send_sync(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    SDBPendingQueueElem *queue_elem = add_pending_id(pkt_in->id);

    pthread_mutex_lock(&queue_elem->queue_pkt.pp_lock);

    if(mcs_write_command(pkt_in, socket_fd) != -1) {
        pthread_cond_wait(&queue_elem->queue_pkt.pp_cond,
                            &queue_elem->queue_pkt.pp_lock);
        *pkt_out = queue_elem->queue_pkt.pp_pkt;
        pthread_mutex_unlock(&queue_elem->queue_pkt.pp_lock);

        delete_pending_id(pkt_in->id);
        return 0;
    } else {
        pthread_mutex_unlock(&queue_elem->queue_pkt.pp_lock);
        return -1;
    }
}

int sdb_send_sync_and_free(MCSPacket *pkt_in, MCSPacket **pkt_out)
{
    int ret;
    ret = sdb_send_sync(pkt_in, pkt_out);
    mcs_free(pkt_in);

    return ret;
}

int sdb_send_async(MCSPacket *pkt_in, SDBPendingPacket **pkt_out)
{
    SDBPendingQueueElem *queue_elem = add_pending_id(pkt_in->id);

    *pkt_out = &queue_elem->queue_pkt;

    return mcs_write_command(pkt_in, socket_fd);
}

int sdb_send_async_and_free(MCSPacket *pkt_in, SDBPendingPacket **pkt_out)
{
    int ret;
    ret = sdb_send_async(pkt_in, pkt_out);
    mcs_free(pkt_in);

    return ret;
}

int sdb_wait_async(SDBPendingPacket *pkt)
{
    pthread_mutex_lock(&pkt->pp_lock);
    if(!pkt->pp_valid) {
        pthread_cond_wait(&pkt->pp_cond, &pkt->pp_lock);
    }
    pthread_mutex_unlock(&pkt->pp_lock);

    return 0;
}

int sdb_check_async(SDBPendingPacket *pkt)
{
    int ret;

    pthread_mutex_lock(&pkt->pp_lock);
    ret = (int)pkt->pp_valid;
    pthread_mutex_unlock(&pkt->pp_lock);

    return ret;
}

void sdb_pending_packet_free(SDBPendingPacket *pkt)
{
    delete_pending_id(pkt->pp_id);
}
