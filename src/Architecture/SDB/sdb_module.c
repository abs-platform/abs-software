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
pthread_key_t sdb_module_info;

static SDBModule *get_info(void)
{
    return (SDBModule *)pthread_getspecific(sdb_module_info);
}

static SDBModule *get_info_id(unsigned int id)
{
    pthread_mutex_lock(&sdb_module_lock);
    if(id >= sdb_module_last) {
        pthread_mutex_unlock(&sdb_module_lock);
        printf_dbg("Index not valid\n");
        return NULL;
    }
    pthread_mutex_unlock(&sdb_module_lock);

    return &sdb_module[id];
}

static void process_pkt_message(MCSPacket *pkt)
{
    SDBModule *mod = get_info();
    unsigned int i;
    size_t list_num;
    unsigned int dest_index;
    const char *dest_name;
    const SDBGroup *groups;

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_MESSAGE]) {
        mcs_write_command_and_free(mcs_err_packet(pkt, ECMDFAULT), mod->wfd);
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
        mcs_write_command_and_free(mcs_err_packet(pkt, EUNDEF), mod->wfd);
        return;
    }
#endif

    if(groups[0] != SDB_GROUP_ANY) {
        /* Can do this because it is an array, not a pointer */
        list_num = sizeof(groups) / sizeof(groups[0]);
        for(i = 0; i < list_num; ++i) {
            if(groups[i] == mod->group) {
                break;
            }
        }

        if(i == list_num) {
            printf_dbg("Not enough privileges\n");
            mcs_write_command_and_free(mcs_err_packet(pkt, EPERM), mod->wfd);
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
        mcs_write_command_and_free(mcs_err_packet(pkt, EBADMODID), mod->wfd);
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
            mcs_write_command_and_free(mcs_err_packet(pkt, EPERM), mod->wfd);
            return;
        }
    }

    /* Finally, send */
    sdb_queue_push_nolock(&mod->queue, sdb_packet(pkt, mod->id));
    sdb_module_write_mcs_packet(pkt, dest_index);
}

static void process_pkt_state(MCSPacket *pkt)
{
    SDBModule *mod = get_info();
    unsigned int i;
    size_t list_num;
    int data_size;
    void *data;
    const struct MCSCommandOptionsStatePerms *perms;

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_STATE]) {
        mcs_write_command_and_free(mcs_err_packet(pkt, ECMDFAULT), mod->wfd);
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
            if(perms[i].group == mod->group) {
                if(perms[i].max_expire > (time_t)pkt->args[0]) {
                    printf_dbg("Not enough privileges\n");
                    mcs_write_command_and_free(mcs_err_packet(pkt, EPERM),
                                                mod->wfd);
                    return;
                }
                break;
            }
        }
    }

    data_size = mcs_command_state_list[pkt->cmd].cmd.response_size;
    data = mcs_command_state_list[pkt->cmd].request(pkt);
    if(data_size == -1) {
        mcs_write_command_and_free(mcs_ok_packet_data(pkt,
                    data, strlen((char *)data) + 1), mod->wfd);
    } else {
        /* Send response */
        mcs_write_command_and_free(mcs_ok_packet_data(pkt, data, data_size),
                                    mod->wfd);
    }
    free(data);
}

static void process_pkt_payload(MCSPacket *pkt)
{
    SDBModule *mod = get_info();

    if(pkt->cmd >= mcs_command_list_size[MCS_TYPE_PAYLOAD]) {
        mcs_write_command_and_free(mcs_err_packet(pkt, ECMDFAULT), mod->wfd);
        return;
    }

    /* Supposing packet is valid, just checking specific configurations */

    printf("Recived the payload request %s\n",
                                mcs_command_payload_list[pkt->cmd].cmd.name);

    sdb_queue_push_nolock(&mod->queue, sdb_packet(pkt, mod->id));
    //sdb_queue_push(&sdb_usb_queue_send, sdb_packet_prio(pkt, mod->id));
}


static void process_pkt_sdb(void)
{
    unsigned int id_origin;
    MCSPacket *pkt;
    SDBPacket *pkt_origin;
    SDBModule *mod = get_info();

    id_origin = *((unsigned int *)mod->data);

    pkt = sdb_module_read_mcs_packet();
    if(pkt == NULL) {
        printf_dbg("Packet is wrong (although it arrived to this stage)\n");
        return;
    }

    if (mcs_is_answer_packet(pkt)) {
        pkt_origin = sdb_queue_get_nolock(&mod->queue, pkt);
        if (pkt_origin == NULL) {
            /* Sending an answer means closing a communication. Sending an
             * error would start an infinite chain of error messages.
             */
            printf_dbg("Unexpected answer\n");
        } else {
            mcs_write_command(pkt, mod->wfd);
            sdb_packet_free(pkt_origin);
        }

        mcs_free(pkt);
    } else {
        sdb_queue_push_nolock(&mod->queue, sdb_packet(pkt, id_origin));
        mcs_write_command(pkt, mod->wfd);
    }
}

static void process_pkt_socket(void)
{
    MCSPacket *pkt;
    SDBPacket *pkt_origin;
    SDBModule *mod = get_info();

    pkt = mcs_read_command(mod->rfd, mod->wfd);
    if(pkt == NULL) {
        printf_dbg("Packet is wrong (although it arrived to this stage)\n");
        return;
    }

    if (mcs_is_answer_packet(pkt)) {
        pkt_origin = sdb_queue_get_nolock(&mod->queue, pkt);
        if (pkt_origin == NULL) {
            printf_dbg("Unexpected answer\n");
            /* Sending an answer means closing a communication. Sending an
             * error would start an infinite chain of error messages.
             */
        } else {
            sdb_module_write_mcs_packet(pkt, pkt_origin->id_process);
            sdb_packet_free(pkt_origin);
        }

        mcs_free(pkt);
    } else {
        switch(pkt->type) {
            case MCS_TYPE_MESSAGE:
                process_pkt_message(pkt);
                break;
            case MCS_TYPE_STATE:
                process_pkt_state(pkt);
                break;
            case MCS_TYPE_PAYLOAD:
                process_pkt_payload(pkt);
                break;
            default:
                printf_dbg("Unhandled type\n");
                break;
        }
    }

    mod->data_socket = false;
}


static void sdb_module_clean(void *arg)
{
    SDBModule *mod = get_info();

    /* arg is not used */
    (void)arg;

    close(mod->rfd);
    if(mod->wfd != mod->rfd) {
        close(mod->wfd);
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

int sdb_module_write_mcs_packet(const MCSPacket *pkt, unsigned int to)
{
    SDBModule *mod = get_info();
    SDBModule *mod_to = get_info_id(to);
    void *pos_pkt;
    void *pos;

    if (mod_to == NULL) {
        return -1;
    }

    if(sizeof(*pkt) + pkt->nargs + pkt->data_size > SDB_MODULE_DATA_SIZE) {
        printf_dbg("Data region is too small\n");
        return -1;
    }

    pthread_mutex_lock(&mod_to->lock);
    while(mod_to->data_valid) {
        pthread_cond_wait(&mod_to->cond_var, &mod_to->lock);
    }

    pos = mod_to->data;

    memcpy(pos, &mod->id, sizeof(mod->id));
    pos += sizeof(mod->id);

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

    mod_to->data_valid = true;

    pthread_cond_broadcast(&mod_to->cond_var);
    pthread_mutex_unlock(&mod_to->lock);

    return 0;
}

MCSPacket *sdb_module_read_mcs_packet(void)
{
    MCSPacket *pkt;
    SDBModule *mod = get_info();

    pthread_mutex_lock(&sdb_module_lock);
    if(mod->id >= sdb_module_last) {
        pthread_mutex_unlock(&sdb_module_lock);
        printf_dbg("Index not valid\n");
        return NULL;
    }
    pthread_mutex_unlock(&sdb_module_lock);

    if(!mod->data_valid) {
        return NULL;
    }

    /* Leave a gap for the first element, the id from the sender */
    pkt = abs_malloccpy(mod->data + sizeof(unsigned int), sizeof(*pkt));
    if(pkt->dest != NULL) {
        pkt->dest = abs_malloccpy(pkt->dest, strlen(pkt->dest) + 1);
    }

    if(pkt->nargs != 0) {
        pkt->args = abs_malloccpy(pkt->args, pkt->nargs);
    }

    if(pkt->data_size != 0) {
        pkt->data = abs_malloccpy(pkt->data, pkt->data_size);
    }

    mod->data_valid = false;

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

    pthread_key_delete(sdb_module_info);
}

void *sdb_module_thread(void *arg)
{
    MCSPacket *pkt;
    SDBModule *mod;
    unsigned int my_id;
    int i;
    size_t text_size;
    char *group;

    my_id = *((unsigned int *)arg);
    mod = &sdb_module[my_id];

    mod->id = my_id;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_cleanup_push(sdb_module_clean, NULL);

    printf_dbg("Thread %d has been created! Welcome on board!\n", my_id);

    pthread_setspecific(sdb_module_info, mod);
    pthread_mutex_init(&mod->lock, NULL);
    pthread_cond_init(&mod->cond_var, NULL);

    sdb_queue_init(&mod->queue);

    pthread_mutex_lock(&mod->lock);

    pkt = mcs_read_command(mod->rfd, mod->wfd);
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
    mod->name = abs_malloc0(text_size);
    strncpy((char *)mod->name, (char *)pkt->data, text_size - 1);

    for(i = 0; i < SDB_GROUP_MAX; ++i) {
        if(strcmp(group, group_conversion[i].str) == 0) {
            mod->group = group_conversion[i].val;
            break;
        }
    }

    if(i == SDB_GROUP_MAX) {
        printf_dbg("Not valid group in handshake\n");
        goto error_pkt;
    }

    mcs_write_command_and_free(mcs_ok_packet(pkt), mod->wfd);
    mcs_free(pkt);

    mod->data = malloc(SDB_MODULE_DATA_SIZE);
    mod->data_valid = false;
    mod->data_socket = false;

    /* Now that everything is ready, wake up the observer to notice us */
    sdb_observer_wake_up();

    /* Notify threads waiting and leave space for sending messages */
    pthread_cond_broadcast(&mod->cond_var);
    pthread_mutex_unlock(&mod->lock);

    /* Mutex is always locked when not waiting to cond variable. */
    pthread_mutex_lock(&mod->lock);
    while(1) {
        pthread_cond_wait(&mod->cond_var, &mod->lock);

        printf_dbg("Module %s has work to do\n", mod->name);

        if(mod->data_valid) {
            /* Socket is full duplex, memory region not */
            process_pkt_sdb();
        } else if(mod->data_socket) {
            process_pkt_socket();
        }
    }

    /* We should never arrive here! */
    goto error_ret;

error_pkt:
    mcs_write_command_and_free(mcs_err_packet(pkt, ECONNREFUSED), mod->wfd);
    mcs_free(pkt);
error_ret:
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
