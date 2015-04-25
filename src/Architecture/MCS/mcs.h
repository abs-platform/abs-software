/*Header file for translating functions*/

#ifndef MCS_H
#define	MCS_H

/*Constants and identifiers*/
#define TYPE_MESSAGE 1
#define TYPE_STATE 2
#define TYPE_PAYLOAD 3
#define COMMAND_TYPE_FIELD 4
#define STATE_RESPONSE_SIZE_FIELD 5
#define INT_SIZE 4
#define FLOAT_SIZE 8
#define STRING_SIZE -1

/*Buffers*/
typedef struct command_message {
    struct cJSON *msg;
    struct command_message *next;
} command_message;

typedef struct command_state {
    struct cJSON *stt;
    struct command_state *next;
} command_state;

typedef struct command_payload {
    struct cJSON *pld;
    struct command_payload *next;
} command_payload;

typedef struct command_list {
    struct command_message *messages;
    struct command_state *states;
    struct command_payload *payloads;
} command_list;

/*Identify an individual json command, returning an integer identifier*/
int identify_type (cJSON *json);

/*Translate to C and print the command depending on the type*/
void translate_message (cJSON *json, FILE *out);
void translate_state (cJSON *json, FILE *out);
void translate_payload (cJSON *json, FILE *out);

/*Take the complete json command list and place in buffers accordingly*/
command_list *read_commands (cJSON *json);

/*Translate and print from the buffers*/
void translate_commands(command_list *commandlist);

#endif
