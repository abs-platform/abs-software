#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <abs.h>
#include <mcs.h>
#include <sdb.h>
#include "sdb_private.h"

SDBModule sdb_module[SDB_MODULE_MAX];
unsigned int sdb_module_last = 0;
pthread_mutex_t sdb_module_lock;

static void reject_pkt_socket(unsigned int id)
{
    MCSPacket *pkt;

    pthread_mutex_lock(&sdb_module_lock);
    if(id >= sdb_module_last) {
        pthread_mutex_unlock(&sdb_module_lock);
        printf_dbg("Index not valid\n");
        return;
    }
    pthread_mutex_unlock(&sdb_module_lock);

    pkt = mcs_read_command(sdb_module[id].rfd, sdb_module[id].wfd);
    mcs_free(pkt);
    mcs_write_command_and_free(mcs_err_packet(EBUSY), sdb_module[id].wfd);
    sdb_module[id].data_socket = false;
}

static void process_pkt_sdb(unsigned int my_id)
{
    unsigned int dest_index;
    MCSPacket *pkt;
    
    dest_index = *((unsigned int *)sdb_module[my_id].data);

    /*
     * This is the only strange ID that could be written using the
     * wrappers.
     */
    if(dest_index == SDB_USB_ID) {
        printf_dbg("Unexpected USB packet\n");
        return;
    }

    pkt = sdb_module_read_mcs_packet(my_id, -1);
    if(pkt == NULL) {
        printf_dbg("Packet is wrong (although it arrived to this stage)\n");
    }

    mcs_write_command(pkt, sdb_module[my_id].wfd);
    mcs_free(pkt);

    while(!sdb_module[my_id].data_socket) {
        pthread_cond_wait(&sdb_module[my_id].cond_var, &sdb_module[my_id].lock);
    }

    pkt = mcs_read_command(sdb_module[my_id].rfd, sdb_module[my_id].wfd);
    sdb_module[my_id].data_socket = false;
    while(pkt->type != MCS_TYPE_OK && pkt->type != MCS_TYPE_OK_DATA &&
                                                pkt->type != MCS_TYPE_ERR) {
        mcs_free(pkt);
        mcs_write_command_and_free(mcs_err_packet(EBUSY), sdb_module[my_id].wfd);
        
        while(!sdb_module[my_id].data_socket) {
            pthread_cond_wait(&sdb_module[my_id].cond_var,
                                                    &sdb_module[my_id].lock);
        }

        pkt = mcs_read_command(sdb_module[my_id].rfd, sdb_module[my_id].wfd);        
        sdb_module[my_id].data_socket = false;
    }

    sdb_module_write_mcs_packet(pkt, dest_index, my_id);
}

static void process_pkt_message(const MCSPacket *pkt, unsigned int my_id)
{
    unsigned int i;
    size_t list_num;
    unsigned int dest_index;
    const char *dest_name;
    const SDBGroup *groups;
    MCSPacket *pkt_res;

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_MESSAGE]) {
        mcs_write_command_and_free(mcs_err_packet(ECMDFAULT),
                                                    sdb_module[my_id].wfd);
        return;
    }

    /* Supposing packet is valid, just checking specific configurations */

    printf_dbg("Received the message %s\n",
                        mcs_command_message_list[pkt->cmd].cmd.name);

    /* Check origin */
    groups = mcs_command_message_list[pkt->cmd].origin_groups;
#ifdef DEBUG
    if(groups == NULL) {
        printf_dbg("Origin groups should not be null\n");
        return;
    }
#endif
    if(groups[0] != SDB_GROUP_ANY) {
        /* Can do this because it is an array, not a pointer */
        list_num = sizeof(groups) / sizeof(groups[0]);
        for(i = 0; i < list_num; ++i) {
            if(groups[i] == sdb_module[my_id].group) {
                break;
            }
        }

        if(i == list_num) {
            printf_dbg("Not enough privileges\n");
            mcs_write_command_and_free(mcs_err_packet(EPERM),
                                                    sdb_module[my_id].wfd);
            return;
        }
    }

    /* Get destination */
    dest_name = mcs_command_message_list[pkt->cmd].destination;
    if(dest_name == NULL) {
        /* TODO */
        return;
    } else if(dest_name[0] == '@') {
        dest_name = pkt->dest;
    }

    pthread_mutex_lock(&sdb_module_lock);
    list_num = sdb_module_last;
    pthread_mutex_unlock(&sdb_module_lock);
    for(i = 0; i < list_num; ++i) {
        if(strcmp(dest_name, sdb_module[i].name) == 0) {
            dest_index = i;
            break;
        }
    }

    if(i == list_num) {
        printf_dbg("Non valid destination\n");
        mcs_write_command_and_free(mcs_err_packet(EBADMODID),
                                                    sdb_module[my_id].wfd);
        return;
    }

    /* Check destination */
    groups = mcs_command_message_list[pkt->cmd].destination_groups;
    if(groups != NULL && groups[0] != SDB_GROUP_ANY) {
        /* Can do this because it is an array, not a pointer */
        /* TODO: Array is not full, might segfault!! */
        list_num = sizeof(groups) / sizeof(groups[0]);
        for(i = 0; i < list_num; ++i) {
            if(groups[i] == sdb_module[dest_index].group) {
                break;
            }
        }

        if(i == list_num) {
            printf_dbg("Not enough privileges\n");
            mcs_write_command_and_free(mcs_err_packet(EPERM),
                                                sdb_module[my_id].wfd);
            return;
        }
    }

    /* Write question and read answer */
    sdb_module_write_mcs_packet(pkt, dest_index, my_id);
    pkt_res = sdb_module_read_mcs_packet(my_id, dest_index);
    mcs_write_command(pkt_res, sdb_module[my_id].wfd);
}

static void process_pkt_state(const MCSPacket *pkt, unsigned int my_id)
{
    unsigned int i;
    size_t list_num;
    int data_size;
    void *data;
    const struct MCSCommandOptionsStatePerms *perms;

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_STATE]) {
        mcs_write_command_and_free(mcs_err_packet(ECMDFAULT),
                                                    sdb_module[my_id].wfd);
        return;
    }

    /* Supposing packet is valid, just checking specific configurations */

    printf("Recived the state request %s\n",
                                mcs_command_state_list[pkt->cmd].cmd.name);

    /* Check expire time */
    perms = mcs_command_state_list[pkt->cmd].expire_group;
    if(perms != NULL) {
        list_num = sizeof(perms) / sizeof(perms[0]);
        for(i = 0; i < list_num; ++i) {
            if(perms[i].group == sdb_module[my_id].group) {
                if(perms[i].max_expire > (time_t)pkt->args[0]) {
                    printf_dbg("Not enough privileges\n");
                    mcs_write_command_and_free(mcs_err_packet(EPERM),
                                                    sdb_module[my_id].wfd);
                    return;
                }
                break;
            }
        }
    }

    data_size = mcs_command_state_list[pkt->cmd].cmd.response_size;
    data = mcs_command_state_list[pkt->cmd].request(pkt);
    if(data_size == -1) {
        mcs_write_command_and_free(
                            mcs_ok_packet_data(data, strlen((char *)data)),
                            sdb_module[my_id].wfd);
    } else {
        /* Send response */
        mcs_write_command_and_free(mcs_ok_packet_data(data, data_size),
                                                        sdb_module[my_id].wfd);
    }
    free(data);
}

static void process_pkt_payload(const MCSPacket *pkt, unsigned int my_id)
{
    MCSPacket *pkt_res;

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_PAYLOAD]) {
        mcs_write_command_and_free(mcs_err_packet(ECMDFAULT),
                                                    sdb_module[my_id].wfd);
        return;
    }

    /* Supposing packet is valid, just checking specific configurations */

    printf("Recived the state request %s\n",
                                mcs_command_payload_list[pkt->cmd].cmd.name);

    /* Write question and read answer */
    //usb_queue_push(pkt, my_id);
    pkt_res = sdb_module_read_mcs_packet(my_id, SDB_USB_ID);
    mcs_write_command(pkt_res, sdb_module[my_id].wfd);

    mcs_free(pkt_res);
}

static void sdb_module_clean(void *arg)
{
    unsigned int my_id;

    my_id = *((unsigned int *)arg);
    close(sdb_module[my_id].rfd);
    if(sdb_module[my_id].wfd != sdb_module[my_id].rfd) {
        close(sdb_module[my_id].wfd);
    }
}

void sdb_module_init(int rfd, int wfd)
{
    unsigned int *id;

    if(rfd < 0 || wfd < 0) {
        printf_dbg("File descriptors are not valid\n");
        goto error_ret;
    }

    id = malloc(sizeof(*id));
    pthread_mutex_lock(&sdb_module_lock);
    *id = sdb_module_last;
    sdb_module_last++;
    pthread_mutex_unlock(&sdb_module_lock);

    sdb_module[*id].rfd = rfd;
    sdb_module[*id].wfd = wfd;

    if(pthread_create(&sdb_module[*id].thread_id, NULL,
                                            sdb_module_thread, id) < 0) {
        printf_dbg("Error creating thread\n");
        goto error_ret;
    }

    return;

error_ret:
    close(rfd);
    close(wfd);
}

int sdb_module_write_mcs_packet(const MCSPacket *pkt, unsigned int to,
                                                        unsigned int from)
{
    void *pos_pkt;
    void *pos;

    pthread_mutex_lock(&sdb_module_lock);
    if(to >= sdb_module_last ||
                        (from > sdb_module_last && from != SDB_USB_ID)) {
        pthread_mutex_unlock(&sdb_module_lock);
        printf_dbg("Index not valid\n");
        return -1;
    }
    pthread_mutex_unlock(&sdb_module_lock);

    if(sizeof(*pkt) + pkt->nargs + pkt->data_size > SDB_MODULE_DATA_SIZE) {
        printf_dbg("Data region is too small\n");
        return -1;
    }

    pthread_mutex_lock(&sdb_module[to].lock);
    while(sdb_module[to].data_valid) {
        pthread_cond_wait(&sdb_module[to].cond_var, &sdb_module[to].lock);
    }

    pos = sdb_module[to].data;

    memcpy(pos, &from, sizeof(from));
    pos += sizeof(from);

    pos_pkt = pos;
    memcpy(pos_pkt, pkt, sizeof(*pkt));
    pos += sizeof(*pkt);

    /*
     * Write destination, arguments and data past the struct, and link the
     * struct to the new arguments, so no dependency problems arise.
     */
    if(pkt->dest != NULL) {
        memcpy(pos, pkt->dest, strlen(pkt->dest) + 1);
        ((MCSPacket *)pos_pkt)->dest = pos;
        pos += strlen(pkt->dest) + 1;
    }

    if(pkt->nargs != 0) {
        memcpy(pos, pkt->args, pkt->nargs);
        ((MCSPacket *)pos_pkt)->args = pos;
        pos += pkt->nargs;
    }

    if(pkt->data_size != 0) {
        memcpy(pos, pkt->data, pkt->data_size);
        ((MCSPacket *)pos_pkt)->data = pos;
    }

    sdb_module[to].data_valid = true;

    pthread_cond_broadcast(&sdb_module[to].cond_var);
    pthread_mutex_unlock(&sdb_module[to].lock);

    return 0;
}

MCSPacket *sdb_module_read_mcs_packet(unsigned int my_id, int from)
{
    unsigned int id;
    MCSPacket *pkt;

    pthread_mutex_lock(&sdb_module_lock);
    if(my_id >= sdb_module_last || (from >= 0 && from != SDB_USB_ID &&
                        (unsigned int)from >= sdb_module_last)) {
        pthread_mutex_unlock(&sdb_module_lock);
        printf_dbg("Index not valid\n");
        return NULL;
    }
    pthread_mutex_unlock(&sdb_module_lock);

    while(!sdb_module[my_id].data_valid) {
        pthread_cond_wait(&sdb_module[my_id].cond_var, &sdb_module[my_id].lock);
        if(sdb_module[my_id].data_valid) {
            id = *((unsigned int *)sdb_module[my_id].data);
            if(from != -1 && id != (unsigned int)from) {
                pkt = mcs_err_packet(EBUSY);
                sdb_module_write_mcs_packet(pkt, id, my_id);
                mcs_free(pkt);
                sdb_module[my_id].data_valid = false;
            }
        }
    }

    /* Leave a gap for the first element, the id from the sender */
    pkt = abs_malloccpy(sdb_module[my_id].data + sizeof(unsigned int),
                                                                sizeof(*pkt));
    if(pkt->dest != NULL) {
        pkt->dest = abs_malloccpy(pkt->dest, strlen(pkt->dest));    
    }

    if(pkt->nargs != 0) {
        pkt->args = abs_malloccpy(pkt->args, pkt->nargs);
    }
    if(pkt->data_size != 0) {
        pkt->data = abs_malloccpy(pkt->data, pkt->data_size);
    }

    sdb_module[my_id].data_valid = false;

    return pkt;
}

void sdb_module_cancel_all(void)
{
    unsigned int list_size;
    int i;

    pthread_mutex_lock(&sdb_module_lock);
    list_size = sdb_module_last;
    pthread_mutex_unlock(&sdb_module_lock);

    for(i = 0; i < (int)list_size; ++i) {
        pthread_cancel(sdb_module[i].thread_id);
        pthread_join(sdb_module[i].thread_id, NULL);
    }
}

void *sdb_module_thread(void *arg)
{
    MCSPacket *pkt;
    unsigned int my_id;
    unsigned int dest_index;
    int i;
    size_t text_size;
    char *group;

    my_id = *((unsigned int *)arg);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(sdb_module_clean, arg);

    printf_dbg("Thread %d has been created! Welcome on board!\n", my_id);

    pthread_mutex_init(&sdb_module[my_id].lock, NULL);
    pthread_cond_init(&sdb_module[my_id].cond_var, NULL);

    pthread_mutex_lock(&sdb_module[my_id].lock);

    pkt = mcs_read_command(sdb_module[my_id].rfd, sdb_module[my_id].wfd);
    if(pkt == NULL) {
        printf_dbg("First packet is wrong\n");
        goto error_ret;
    }

    if(mcs_command(pkt) != MCS_MESSAGE_SDB_HANDSHAKE) {
        printf_dbg("Unexpected packet\n");
        goto error_pkt;
    }

    group = strchr((char *)pkt->data, ':');
    if(group == NULL) {
        printf_dbg("Not valid handshake\n");
        goto error_pkt;
    }

    *group = '\0';
    group++;

    if(((char *)pkt->data - group - 1) >= SDB_MODULE_NAME_SIZE) {
        printf_dbg("Name too long\n");
        goto error_pkt;
    }

    /* Remember NULL character! */
    text_size = (size_t)(group - (char *)pkt->data);
    sdb_module[my_id].name = abs_malloc0(text_size);
    strncpy((char *)sdb_module[my_id].name, (char *)pkt->data, text_size - 1);

    for(i = 0; i < SDB_GROUP_MAX; ++i) {
        if(strcmp(group, group_conversion[i].str) == 0) {
            sdb_module[my_id].group = group_conversion[i].val;
            break;
        }
    }

    if(i == SDB_GROUP_MAX) {
        printf_dbg("Not valid group in handshake\n");
        goto error_pkt;
    }

    mcs_free(pkt);
    mcs_write_command_and_free(mcs_ok_packet(), sdb_module[my_id].wfd);

    sdb_module[my_id].data = malloc(SDB_MODULE_DATA_SIZE);
    sdb_module[my_id].data_valid = false;
    sdb_module[my_id].data_socket = false;

    /* Now that everything is ready, wake up the observer to notice us */
    sdb_observer_wake_up();

    /* Notify threads waiting and leave space for sending messages */
    pthread_cond_broadcast(&sdb_module[my_id].cond_var);
    pthread_mutex_unlock(&sdb_module[my_id].lock);

    /* Mutex is always locked when not waiting to cond variable. */
    pthread_mutex_lock(&sdb_module[my_id].lock);
    while(1) {
        while(!sdb_module[my_id].data_valid &&
                                        !sdb_module[my_id].data_socket) {
            pthread_cond_wait(&sdb_module[my_id].cond_var, 
                                                &sdb_module[my_id].lock);
        }

        printf_dbg("Module %s has work to do\n", sdb_module[my_id].name);
        if(sdb_module[my_id].data_valid && sdb_module[my_id].data_socket) {
            /* Data collision, check priorities */
            dest_index = *((unsigned int *)sdb_module[my_id].data);

            /*
             * This is the only strange ID that could be written using the
             * wrappers.
             */
            if(dest_index == SDB_USB_ID) {
                printf_dbg("Unexpected USB packet\n");
                continue;
            }

            if(sdb_group_priority[sdb_module[my_id].group] <
                            sdb_group_priority[sdb_module[dest_index].group]) {
                /* Request through socket is more important */
                pkt = mcs_err_packet(EBUSY);
                sdb_module_write_mcs_packet(pkt, dest_index, my_id);
                mcs_free(pkt);
            } else {
                /* Request through SDB is more important */
                reject_pkt_socket(my_id);
            }
        }

        /*
         * Mutex is locked. No new data should have arrived between last
         * conditional and this conditional.
         */
        if(sdb_module[my_id].data_valid) {
            process_pkt_sdb(my_id);
        } else if(sdb_module[my_id].data_socket) {
            pkt = mcs_read_command(sdb_module[my_id].rfd,
                                                    sdb_module[my_id].wfd);
            if(pkt != NULL) {
                switch(pkt->type) {
                    case MCS_TYPE_MESSAGE:
                        process_pkt_message(pkt, my_id);
                        break;
                    case MCS_TYPE_STATE:
                        process_pkt_state(pkt, my_id);
                        break;
                    case MCS_TYPE_PAYLOAD:
                        process_pkt_payload(pkt, my_id);
                        break;
                    default:
                        printf_dbg("Type is not valid\n");
                        mcs_write_command_and_free(mcs_err_packet(EUNDEF), 
                                                        sdb_module[my_id].wfd);
                        break;
                }

                sdb_module[my_id].data_socket = false;

                mcs_free(pkt);
            } else {
                printf_dbg("There should be data in the socket, "
                                                        "but there isn't\n");
            }
        }
    }

    /* We should never arrive here! */
    goto error_ret;

error_pkt:
    mcs_write_command_and_free(mcs_err_packet(ECONNREFUSED),
                                                    sdb_module[my_id].wfd);
    mcs_free(pkt);
error_ret:
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
