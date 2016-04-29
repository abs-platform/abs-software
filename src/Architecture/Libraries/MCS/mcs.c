#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <abs.h>
#include <mcs.h>
#include <sdb.h>

static unsigned int generate_id(void)
{
    return (unsigned int)random();
}

static int read_check_config(int fd, MCSPacket *pkt)
{
    int ret;
    unsigned char dest_size;
    const MCSCommandOptionsCommon *cmd;

    if(pkt->type >= MCS_COMMAND_TYPES) {
        printf_dbg("The type of command %u does not exist\n", pkt->type);
        return -1;
    }

    if(pkt->cmd >= mcs_command_list_size[pkt->type]) {
        printf_dbg("Command %u of type %u does not exist\n",
                                            pkt->cmd, pkt->type);
        return -1;
    }

    if(pkt->type == MCS_TYPE_MESSAGE) {
        cmd = &mcs_command_message_list[pkt->cmd].cmd;
    } else if(pkt->type == MCS_TYPE_STATE) {
        cmd = &mcs_command_state_list[pkt->cmd].cmd;
    } else if(pkt->type == MCS_TYPE_PAYLOAD) {
        cmd = &mcs_command_payload_list[pkt->cmd].cmd;
    }

    pkt->nargs = cmd->nargs;

    /* Read arguments */
    if(pkt->nargs > 0) {
        pkt->args = malloc(pkt->nargs);
        ret = abs_read(fd, (char *)pkt->args, pkt->nargs, MCS_READ_TIMEOUT_US);
        if(ret < pkt->nargs) {
            printf_dbg("Could not read. Expecting %d arguments. Returned %d\n",
                                        pkt->nargs, ret);
            return -1;
        }
    }

    /* Read destination */
    if(pkt->type == MCS_TYPE_MESSAGE &&
                mcs_command_message_list[pkt->cmd].destination != NULL &&
                mcs_command_message_list[pkt->cmd].destination[0] == '@') {
        if(abs_read(fd, (char *)&dest_size, sizeof(dest_size),
                        MCS_READ_TIMEOUT_US) < (int)sizeof(dest_size)) {
            printf_dbg("Could not read destination size\n");
            return -1;
        }

        if(dest_size > SDB_MODULE_NAME_SIZE) {
            printf_dbg("Name too long\n");
            return -1;
        }

        pkt->dest = malloc(dest_size + 1);
        if(abs_read(fd, pkt->dest, dest_size, MCS_READ_TIMEOUT_US)
                                                    < dest_size) {
            printf_dbg("Could not read destination\n");
            return -1;
        }

        pkt->dest[dest_size] = '\0'; /* Is a string, NULL terminated */
    }

    /* Read raw data */
    if(cmd->raw_data) {
        if(abs_read(fd, (char *)&pkt->data_size, sizeof(pkt->data_size),
                        MCS_READ_TIMEOUT_US) < (int)sizeof(pkt->data_size)) {
            printf_dbg("Could not read raw data size\n");
            return -1;
        }

        pkt->data = malloc(pkt->data_size);
        if(abs_read(fd, (char *)pkt->data, pkt->data_size, MCS_READ_TIMEOUT_US)
                                                < (int)pkt->data_size) {
            printf_dbg("Could not read raw data\n");
            return -1;
        }
    }

    return 0;
}

static int check_packet(MCSPacket *pkt)
{
    const MCSCommandOptionsCommon *cmd;

    if(pkt->type == MCS_TYPE_ERR || pkt->type == MCS_TYPE_OK ||
                                            pkt->type == MCS_TYPE_OK_DATA) {
        if(pkt->nargs != 0) {
            printf_dbg("Packet type ERR and OK should not have arguments\n");
            return -1;
        }

        if(pkt->type == MCS_TYPE_OK_DATA && pkt->data_size == 0) {
            printf_dbg("Packet type OK_DATA should have data attached\n");
            return -1;
        }

        if(pkt->type == MCS_TYPE_OK && pkt->data_size != 0) {
            printf_dbg("Packet type OK should write the data size\n");
            return -1;
        }

#ifdef DEBUG
        /* This is for extreme compliance */
        if(pkt->cmd != 0) {
            printf_dbg("Command in packet type ERR and OK should be 0\n");
        }
#endif
    } else {
        if(pkt->type >= MCS_COMMAND_TYPES) {
            printf_dbg("The type of command %u does not exist\n", pkt->type);
            return -1;
        }

        if(pkt->cmd >= mcs_command_list_size[pkt->type]) {
            printf_dbg("Command %u of type %u does not exist\n",
                                                pkt->cmd, pkt->type);
            return -1;
        }

        if(pkt->type == MCS_TYPE_MESSAGE) {
            cmd = &mcs_command_message_list[pkt->cmd].cmd;
        } else if(pkt->type == MCS_TYPE_STATE) {
            cmd = &mcs_command_state_list[pkt->cmd].cmd;
        } else if(pkt->type == MCS_TYPE_PAYLOAD) {
            cmd = &mcs_command_payload_list[pkt->cmd].cmd;
        }

        if(cmd->nargs != pkt->nargs) {
            printf_dbg("Arguments for command %s do not follow specs\n",
                                                            cmd->name);
            return -1;
        }

        if((cmd->raw_data && pkt->data_size == 0) ||
            (!cmd->raw_data && pkt->data_size != 0)) {
            printf_dbg("Raw data for command %s does not follow specs\n",
                                                            cmd->name);
            return -1;
        }
    }

    if(pkt->nargs != 0 && pkt->args == NULL) {
        printf_dbg("Mismatch between pkt->nargs and pkt->args\n");
        return -1;
    }

    if(pkt->data_size != 0 && pkt->data == NULL) {
        printf_dbg("Mismatch between pkt->data_size and pkt->data\n");
        return -1;
    }

#ifdef DEBUG
    /* This is for extreme compliance */
    if(pkt->nargs == 0 && pkt->args != NULL) {
        printf_dbg("Mismatch between pkt->nargs and pkt->args\n");
        return -1;
    }

    if(pkt->data_size == 0 && pkt->data != NULL) {
        printf_dbg("Mismatch between pkt->data_size and pkt->data\n");
        return -1;
    }
#endif

    return 0;
}

void mcs_init(void)
{
    srandom(time(NULL) * getpid());
}

void mcs_free(MCSPacket *pkt)
{
    if(pkt == NULL) {
        return;
    }

#ifdef DEBUG
    /* This is for extreme compliance */
    if(pkt->nargs == 0 && pkt->args != NULL) {
        printf_dbg("Mismatch between pkt->nargs and pkt->args\n");
        free(pkt->args);
    }

    if(pkt->nargs != 0 && pkt->args == NULL) {
        printf_dbg("Mismatch between pkt->nargs and pkt->args\n");
        pkt->nargs = 0;
    }

    if(pkt->data_size == 0 && pkt->data != NULL) {
        printf_dbg("Mismatch between pkt->data_size and pkt->data\n");
        free(pkt->data);
    }

    if(pkt->data_size != 0 && pkt->data == NULL) {
        printf_dbg("Mismatch between pkt->data_size and pkt->data\n");
        pkt->data_size = 0;
    }
#endif

    if(pkt->nargs != 0) {
        free(pkt->args);
    }

    if(pkt->data_size != 0) {
        free(pkt->data);
    }

    free(pkt);
}

/*
 * Read a command from a given file descriptor and translate it into a
 * MCSPacket
 */
MCSPacket *mcs_read_command(int rfd, int wfd)
{
    MCSPacket *pkt;

    if(rfd < 0) {
        printf_dbg("FD not valid\n");
        return NULL;
    }

    pkt = abs_malloc0(sizeof(*pkt));

    if(abs_read(rfd, (char *)&pkt->id, sizeof(pkt->id), MCS_READ_TIMEOUT_US)
                                                    < (int)sizeof(pkt->id)) {
        printf_dbg("Could not read packet ID\n");
        goto error_no_notify;
    }

    if(abs_read(rfd, (char *)&pkt->type, sizeof(char), MCS_READ_TIMEOUT_US)
                                                    < (int)sizeof(char)) {
        printf_dbg("Could not read packet type\n");
        goto error;
    }

    switch(pkt->type) {
        case MCS_TYPE_MESSAGE:
        case MCS_TYPE_STATE:
        case MCS_TYPE_PAYLOAD:
            if(abs_read(rfd, (char *)&pkt->cmd, sizeof(pkt->cmd),
                                MCS_READ_TIMEOUT_US) < (int)sizeof(pkt->cmd)) {
                printf_dbg("Could not read command\n");
                goto error;
            }

            if(read_check_config(rfd, pkt) < 0) {
                goto error;
            }
            break;

        case MCS_TYPE_ERR:
            pkt->data_size = sizeof(int);
            pkt->data = malloc(pkt->data_size);
            if(abs_read(rfd, (char *)pkt->data, pkt->data_size,
                                MCS_READ_TIMEOUT_US) < pkt->data_size) {
                printf_dbg("Could not read error code\n");
                goto error;
            }
            break;

        case MCS_TYPE_OK_DATA:
            if(abs_read(rfd, (char *)&pkt->data_size, sizeof(pkt->data_size),
                        MCS_READ_TIMEOUT_US) < (int)sizeof(pkt->data_size)) {
                printf_dbg("Could not read raw data size\n");
                goto error;
            }

            pkt->data = malloc(pkt->data_size);
            if(abs_read(rfd, (char *)pkt->data, pkt->data_size,
                                MCS_READ_TIMEOUT_US) < pkt->data_size) {
                printf_dbg("Could not read raw data\n");
                goto error;
            }
            break;

        case MCS_TYPE_OK:
            break;

        default:
            printf_dbg("Non valid command\n");
            goto error;
    }

    return pkt;

error:
    mcs_write_command_and_free(mcs_err_packet(pkt, EBADFORMAT), wfd);
    mcs_free(pkt);
error_no_notify:
    return NULL;
}

int mcs_write_command(MCSPacket *pkt, int fd)
{
    size_t common_size, dest_size;
    size_t tot_size;
    char *raw, *raw_act;

    if(check_packet(pkt) < 0) {
        return EUNDEF;
    }

    common_size = sizeof(pkt->id) + sizeof(char);

    switch(pkt->type) {
        case MCS_TYPE_ERR:
            tot_size = common_size + pkt->data_size;
            raw = malloc(tot_size);
            memcpy(raw, (char *)&pkt->id, common_size);
            memcpy(raw + common_size, pkt->data, pkt->data_size);
            break;

        case MCS_TYPE_OK_DATA:
            tot_size = common_size + sizeof(pkt->data_size) + pkt->data_size;
            raw = malloc(tot_size);
            raw_act = raw;
            memcpy(raw_act, (char *)&pkt->id, common_size);
            raw_act += common_size;
            memcpy(raw_act, &pkt->data_size, sizeof(pkt->data_size));
            raw_act += sizeof(pkt->data_size);
            memcpy(raw_act, pkt->data, pkt->data_size);
            raw_act += pkt->data_size;

#ifdef DEBUG
            if((size_t)(raw_act - raw) != tot_size) {
                printf_dbg("Real size is not expected size\n");
                return EUNDEF;
            }
#endif

            break;

        case MCS_TYPE_OK:
            tot_size = common_size;
            raw = malloc(tot_size);
            memcpy(raw, (char *)&pkt->id, common_size);
            break;

        default:
            tot_size = common_size + sizeof(pkt->cmd) + pkt->nargs;

            if(pkt->dest != NULL) {
                tot_size += sizeof(unsigned char);
                tot_size += strlen(pkt->dest);
            }

            if(pkt->data_size != 0) {
                tot_size += sizeof(pkt->data_size) + pkt->data_size;
            }

            raw = malloc(tot_size);
            raw_act = raw;

            memcpy(raw_act, (char *)&pkt->id, common_size);
            raw_act += common_size;
            memcpy(raw_act, &pkt->cmd, sizeof(pkt->cmd));
            raw_act += sizeof(pkt->cmd);
            if(pkt->nargs != 0) {
                memcpy(raw_act, pkt->args, pkt->nargs);
                raw_act += pkt->nargs;
            }

            if(pkt->dest != NULL) {
                dest_size = strlen(pkt->dest);
                memcpy(raw_act, (unsigned char *)&dest_size,
                                                    sizeof(unsigned char));
                raw_act += sizeof(unsigned char);
                memcpy(raw_act, pkt->dest, dest_size);
                raw_act += dest_size;
            }

            if(pkt->data_size != 0) {
                memcpy(raw_act, &pkt->data_size, sizeof(pkt->data_size));
                raw_act += sizeof(pkt->data_size);
                memcpy(raw_act, pkt->data, pkt->data_size);
                raw_act += pkt->data_size;
            }

#ifdef DEBUG
            if((size_t)(raw_act - raw) != tot_size) {
                printf_dbg("Real size is not expected size\n");
                return EUNDEF;
            }
#endif
            break;
    }

    if(abs_write(fd, raw, tot_size) < (int)tot_size) {
        return EUNDEF;
    }

    free(raw);

    return 0;
}

int mcs_write_command_and_free(MCSPacket *pkt, int fd)
{
    int ret = mcs_write_command(pkt, fd);
    mcs_free(pkt);
    return ret;
}

MCSPacket *mcs_ok_packet_data_id(unsigned int id, void *data, size_t size)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->id = id;
    pkt->type = MCS_TYPE_OK_DATA;
    pkt->data_size = size;
    if(size != 0) {
        pkt->data = abs_malloccpy(data, size);
    }

    return pkt;
}

MCSPacket *mcs_ok_packet_id(unsigned int id)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->id = id;
    pkt->type = MCS_TYPE_OK;

    return pkt;
}

MCSPacket *mcs_err_packet_id(unsigned int id, int err_code)
{
    MCSPacket *pkt;
    int *err_code_mem;

    err_code_mem = malloc(sizeof(*err_code_mem));
    *err_code_mem = err_code;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->id = id;
    pkt->type = MCS_TYPE_ERR;
    pkt->data_size = sizeof(int);
    pkt->data = (unsigned char *)err_code_mem;

    return pkt;
}

MCSPacket *mcs_create_packet(MCSCommand cmd, unsigned short nargs,
        unsigned char *args, unsigned short data_size, unsigned char *data)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->type = (cmd & 0xF0000) >> 16;
    pkt->cmd = cmd & 0xFFFF;

    if (pkt->type == MCS_TYPE_PAYLOAD) {
        pkt->id = (unsigned char)(generate_id()) & 0x7F;
    } else {
        pkt->id = generate_id();
    }

    pkt->nargs = nargs;
    if(nargs != 0) {
        pkt->args = abs_malloccpy(args, nargs);
    }
    pkt->data_size = data_size;
    if(data_size != 0) {
        pkt->data = abs_malloccpy(data, data_size);
    }

    if(check_packet(pkt) >= 0) {
        return pkt;
    } else {
        return NULL;
    }
}

MCSPacket *mcs_create_packet_with_dest(MCSCommand cmd, char *dest,
                            unsigned short nargs, unsigned char *args,
                            unsigned short data_size, unsigned char *data)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->id = generate_id();
    pkt->type = (cmd & 0xF0000) >> 16;
    pkt->cmd = cmd & 0xFFFF;
    pkt->dest = dest;
    pkt->nargs = nargs;
    if(nargs != 0) {
        pkt->args = abs_malloccpy(args, nargs);
    }
    pkt->data_size = data_size;
    if(data_size != 0) {
        pkt->data = abs_malloccpy(data, data_size);
    }

    if(check_packet(pkt) >= 0) {
        return pkt;
    } else {
        return NULL;
    }
}

int mcs_err_code_from_command(const MCSPacket *pkt)
{
    int *err_code;
    if(pkt->data_size == sizeof(int)) {
        err_code = (int *)pkt->data;
        return *err_code;
    } else {
        return 0;
    }
}

const char *mcs_command_to_string(const MCSPacket *pkt)
{
    static const char *err = "error";
    static const char *ok = "ok";
    static const char *ok_data = "ok_data";

    if(pkt->type == MCS_TYPE_ERR) {
        return err;
    } else if(pkt->type == MCS_TYPE_OK) {
        return ok;
    } else if(pkt->type == MCS_TYPE_OK_DATA) {
        return ok_data;
    } else if(pkt->type < MCS_COMMAND_TYPES &&
                        pkt->cmd < mcs_command_list_size[pkt->type]) {
        if(pkt->type == MCS_TYPE_MESSAGE) {
            return mcs_command_message_list[pkt->cmd].cmd.name;
        } else if(pkt->type == MCS_TYPE_STATE) {
            return mcs_command_state_list[pkt->cmd].cmd.name;
        } else if(pkt->type == MCS_TYPE_PAYLOAD) {
            return mcs_command_payload_list[pkt->cmd].cmd.name;
        }
    }

    return NULL;
}

bool mcs_is_answer_packet(const MCSPacket *pkt)
{
    return (pkt->type == MCS_TYPE_OK || pkt->type == MCS_TYPE_OK_DATA ||
            pkt->type == MCS_TYPE_ERR);
}
