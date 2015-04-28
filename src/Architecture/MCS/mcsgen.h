/*Header file for translating functions*/

#ifndef __MCSG_H
#define __MCSG_H

#define COMMAND_TYPE_FIELD 4               //MODIFY: MCSGEN? prefix before constants
#define STATE_RESPONSE_SIZE_FIELD 5
#define INT_SIZE 4
#define FLOAT_SIZE 8
#define STRING_SIZE -1
#define TYPE_MESSAGE_DEC 0
#define TYPE_STATE_DEC 65536
#define TYPE_PAYLOAD_DEC 131072

#define MCS_TYPE_MESSAGE     0
#define MCS_TYPE_STATE       1
#define MCS_TYPE_PAYLOAD     2

#include "../include/mcs.h"

/*Buffers*/
typedef struct MCSGenCommand {
    struct cJSON *cmd;
    struct MCSGenCommand *next;
} MCSGenCommand;

typedef struct MCSGenCommandList {
    MCSGenCommand *messages;
    MCSGenCommand *states;
    MCSGenCommand *payloads;
} MCSGenCommandList;

/*Identify an individual json command, returning an integer identifier*/
int identify_type(cJSON *json);

/*Translate to C and print the command depending on the type*/
void translate_message(cJSON *json, FILE *out);
void translate_state(cJSON *json, FILE *out);
void translate_payload(cJSON *json, FILE *out);

/*Take the complete json command list and place in buffers accordingly*/
MCSGenCommandList *read_commands(cJSON *json);

void translate_enum(cJSON *json, FILE *out, int command_count, int type_value);

/*Translate and print from the buffers*/
void translate_commands(MCSGenCommandList *commandlist);

#endif
