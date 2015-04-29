/*Header file for translating functions*/

#ifndef __MCSG_H
#define __MCSG_H

#define MCSG_COMMAND_TYPE_FIELD 4
#define MCSG_STATE_RESPONSE_SIZE_FIELD 5
#define MCSG_INT_SIZE 4
#define MCSG_FLOAT_SIZE 8
#define MCSG_STRING_SIZE -1
#define MCSG_TYPE_MESSAGE_DEC 0
#define MCSG_TYPE_STATE_DEC 65536
#define MCSG_TYPE_PAYLOAD_DEC 131072

#define MCS_TYPE_MESSAGE   0     /*      Temporary constants until         */
#define MCS_TYPE_STATE     1     /*      #include "../include/mcs.h        */
#define MCS_TYPE_PAYLOAD   2     /*      works properly                    */

#include "../include/mcs.h"

/*Buffers*/
typedef struct MCSGCommand {
    struct cJSON *cmd;
    struct MCSGCommand *next;
} MCSGCommand;

typedef struct MCSGCommandList {
    MCSGCommand *messages;
    MCSGCommand *states;
    MCSGCommand *payloads;
} MCSGCommandList;

/*Identify an individual json command, returning an integer identifier*/
int mcsg_type_identifier(cJSON *json);

/*Translate to C and print the command depending on the type*/
void mcsg_message_translator(cJSON *json, FILE *out);
void mcsg_state_translator(cJSON *json, FILE *out);
void mcsg_payload_translator(cJSON *json, FILE *out);

/*Take the complete json command list and place in buffers accordingly*/
MCSGCommandList *mcsg_commands_reader(cJSON *json);

/*Translate and print a component from the enum list*/
void mcsg_enum_translator(cJSON *json, FILE *out, int command_count, int type_value);

/*Translate and print from the buffers*/
void mcsg_commands_translator(MCSGCommandList *commandlist);

#endif
