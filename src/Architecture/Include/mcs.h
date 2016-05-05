#ifndef __MCS_H
#define __MCS_H

#include <stdbool.h>
#include <sdb_group.h>

#define MCS_READ_TIMEOUT_US 100000
#define MCS_COMMAND_TYPES 3

enum MCSType {
    MCS_TYPE_MESSAGE    = 0,
    MCS_TYPE_STATE      = 1,
    MCS_TYPE_PAYLOAD    = 2,
    MCS_TYPE_OK         = 253,
    MCS_TYPE_OK_DATA    = 254,
    MCS_TYPE_ERR        = 255,
};

/* Datatype for the packets that can be sent to and from the SDB */
typedef struct MCSPacket {
    unsigned int id;
    enum MCSType type;
    unsigned short cmd;
    unsigned short nargs;
    unsigned char *args;
    char *dest;
    unsigned short data_size;
    unsigned char *data;
} MCSPacket;

/* Datatypes for the autogenerated lists of commands */
typedef struct MCSCommandOptionsCommon {
    const char *name;
    unsigned short nargs;
    bool raw_data;
    int response_size;
} MCSCommandOptionsCommon;

struct MCSCommandOptionsMessage {
    struct MCSCommandOptionsCommon cmd;
    const char *destination;
    SDBGroup origin_groups[SDB_GROUP_MAX];
    SDBGroup destination_groups[SDB_GROUP_MAX];
};

struct MCSCommandOptionsStatePerms {
    SDBGroup group;
    time_t max_expire;
};

struct MCSCommandOptionsState {
    struct MCSCommandOptionsCommon cmd;
    void *(*request)(const MCSPacket *pkt);
    unsigned int dimensions;
    struct MCSCommandOptionsStatePerms expire_group[SDB_GROUP_MAX];
};

struct MCSCommandOptionsPayload {
    struct MCSCommandOptionsCommon cmd;
    const char command;
    const char parameters;
    const char *arguments;
    const char *data;
};

#ifndef NO_AUTO_MCS
// DEBUG!
static inline void *get_sensor_value_arduino()
{
    return NULL;
}

/* The autogenerated lists of commands */
#include "auto_mcs.h"

static const int mcs_command_list_size[] =
{
    MCS_COMMAND_MESSAGE_LIST_SIZE,
    MCS_COMMAND_STATE_LIST_SIZE,
    MCS_COMMAND_PAYLOAD_LIST_SIZE,
};

/* Will be called when the library is loaded */
void mcs_init(void) __attribute__((constructor));

void mcs_free(MCSPacket *pkt);

MCSPacket *mcs_read_command(int rfd, int wfd);
int mcs_write_command(MCSPacket *pkt, int fd);
int mcs_write_command_and_free(MCSPacket *pkt, int fd);

MCSPacket *mcs_ok_packet_data_id(unsigned int id, void *data, size_t size);
MCSPacket *mcs_ok_packet_id(unsigned int id);
MCSPacket *mcs_err_packet_id(unsigned int id, int err_code);
MCSPacket *mcs_create_packet(MCSCommand cmd, unsigned short nargs,
        unsigned char *args, unsigned short data_size, unsigned char *data);
MCSPacket *mcs_create_packet_with_dest(MCSCommand cmd, char *dest,
                            unsigned short nargs, unsigned char *args,
                            unsigned short data_size, unsigned char *data);

int mcs_err_code_from_command(const MCSPacket *pkt);
const char *mcs_command_to_string(const MCSPacket *pkt);
bool mcs_is_answer_packet(const MCSPacket *pkt);

static inline MCSCommand mcs_command(MCSPacket *pkt)
{
    return ((unsigned int)pkt->type << 2) | (unsigned int)pkt->cmd;
}

static inline MCSPacket *mcs_ok_packet_data(const MCSPacket *from, void *data,
                                            size_t size)
{
    return mcs_ok_packet_data_id(from->id, data, size);
}

static inline MCSPacket *mcs_ok_packet(const MCSPacket *from)
{
    return mcs_ok_packet_id(from->id);
}

static inline MCSPacket *mcs_err_packet(const MCSPacket *from, int err_code)
{
    return mcs_err_packet_id(from->id, err_code);
}
#endif

#endif
