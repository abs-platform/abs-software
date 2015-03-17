#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <abs.h>
#include <mcs.h>

/* #define DEBUG 1 */

static int read_check_config(int fd, MCSPacket *pkt)
{
    const MCSCommandOptionsCommon *cmd;

    if(pkt->type >= mcs_command_types) {
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

    /* Last argument is the raw data */
    if(cmd->raw_data) {
        pkt->nargs -= 1;
    }

    /* Read arguments */
    if(pkt->nargs > 0) {
        pkt->args = malloc(pkt->nargs);
        if(read(fd, pkt->args, pkt->nargs) < pkt->nargs) {
            printf_dbg("Could not read");
            return -1;
        }
    }

    /* Read raw data */
    if(cmd->raw_data) {
        if(read(fd, &pkt->data_size, sizeof(pkt->data_size))
                                                    < sizeof(pkt->data_size)) {
            printf_dbg("Could not read\n");
            return -1;
        }

        pkt->data = malloc(pkt->data_size);
        if(read(fd, &pkt->data, pkt->data_size) < pkt->data_size) {
            printf_dbg("Could not read\n");
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
        if(pkt->type >= mcs_command_types) {
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
MCSPacket *mcs_read_command(int fd)
{
    MCSPacket *pkt;
    
    if(fd < 0) {
        printf_dbg("FD not valid\n");
        return NULL;
    }

    pkt = abs_malloc0(sizeof(*pkt));

    if(read(fd, (char *)&pkt->type, sizeof(char)) < sizeof(char)) {
        printf_dbg("Could not read\n");
        goto error;
    }

    switch(pkt->type) {
        case MCS_TYPE_MESSAGE:
        case MCS_TYPE_STATE:
        case MCS_TYPE_PAYLOAD:
            if(read(fd, &pkt->cmd, sizeof(pkt->cmd)) < sizeof(pkt->cmd)) {
                printf_dbg("Could not read\n");
                goto error;
            }

            if(read_check_config(fd, pkt) < 0) {
                goto error;
            }
            break;

        case MCS_TYPE_ERR:
            pkt->data_size = sizeof(MCS_error_code);
            pkt->data = malloc(pkt->data_size);
            if(read(fd, pkt->data, pkt->data_size) < pkt->data_size) {
                printf_dbg("Could not read\n");
                goto error;
            }
            break;

        case MCS_TYPE_OK_DATA:
            if(read(fd, &pkt->data_size, sizeof(pkt->data_size))
                                                    < sizeof(pkt->data_size)) {
                printf_dbg("Could not read\n");
                goto error;
            }

            pkt->data = malloc(pkt->data_size);
            if(read(fd, pkt->data, pkt->data_size) < pkt->data_size) {
                printf_dbg("Could not read\n");
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
    mcs_free(pkt);
    return NULL;
}

int mcs_write_command(int fd, MCSPacket *pkt)
{
    size_t type_size, cmd_size, args_size, data_size_size, data_size;
    size_t tot_size;
    char *raw;

    if(check_packet(pkt) < 0) {
        return -1;
    }

    type_size = sizeof(char);
    cmd_size = sizeof(pkt->cmd);
    args_size = pkt->nargs * sizeof(*pkt->args);
    data_size_size = sizeof(pkt->data_size);
    data_size = pkt->data_size * sizeof(*pkt->data);

    switch(pkt->type) {
        case MCS_TYPE_ERR:
            tot_size = type_size + data_size;
            raw = malloc(tot_size);
            memcpy(raw, (char *)&pkt->type, type_size);
            memcpy(raw + type_size, pkt->data, data_size);
            break;

        case MCS_TYPE_OK_DATA:
            tot_size = type_size + data_size_size + data_size;
            raw = malloc(tot_size);
            memcpy(raw, (char *)&pkt->type, type_size);
            memcpy(raw + type_size, &pkt->data_size, data_size_size);
            memcpy(raw + type_size + data_size_size, pkt->data, data_size);
            break;

        case MCS_TYPE_OK:
            tot_size = type_size;
            raw = malloc(tot_size);
            memcpy(raw, (char *)&pkt->type, type_size);
            break;

        default:
            tot_size = type_size + cmd_size + args_size;
            
            if(data_size != 0) {
                tot_size += data_size_size + data_size;
            }

            raw = malloc(tot_size);

            memcpy(raw, (char *)&pkt->type, type_size);
            memcpy(raw + type_size, &pkt->cmd, cmd_size);
            memcpy(raw + type_size + cmd_size, pkt->args, args_size);

            if(data_size != 0) {
                memcpy(raw + type_size + cmd_size + args_size,
                                            &pkt->data_size, data_size_size);
                memcpy(raw + type_size + cmd_size + args_size + data_size_size,
                                            pkt->data, data_size);
            }
            break;
    }

    if(write(fd, raw, tot_size) < tot_size) {
        return -1;
    }

    free(raw);

    return 0;
}

int mcs_write_command_and_free(int fd, MCSPacket *pkt)
{
    int ret = mcs_write_command(fd, pkt);
    mcs_free(pkt);
    return ret;
}

MCSPacket *mcs_ok_packet_data(void *data, size_t size)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->type = MCS_TYPE_OK_DATA;
    pkt->data_size = size;
    if(size != 0) {
        pkt->data = abs_malloccpy(data, size);    
    }

    return pkt;
}

MCSPacket *mcs_ok_packet(void)
{
    MCSPacket *pkt;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->type = MCS_TYPE_OK;

    return pkt;
}

MCSPacket *mcs_err_packet(MCS_error_code err_code)
{
    MCSPacket *pkt;
    MCS_error_code *err_code_mem;
    
    err_code_mem = malloc(sizeof(*err_code_mem));
    *err_code_mem = err_code;

    pkt = abs_malloc0(sizeof(*pkt));
    pkt->type = MCS_TYPE_ERR;
    pkt->data_size = sizeof(MCS_error_code);
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

MCS_error_code mcs_err_code_from_command(MCSPacket *pkt)
{
    MCS_error_code *err_code;
    if(pkt->data_size == sizeof(MCS_error_code)) {
        err_code = (MCS_error_code *)pkt->data;
        return *err_code;
    } else {
        return 0;
    }
}

const char *mcs_command_to_string(MCSPacket *pkt)
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
    } else if(pkt->type < mcs_command_types &&
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
