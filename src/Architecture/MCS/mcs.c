#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cJSON.h"
#include "mcs.h"

#define TYPE_MESSAGE 1
#define TYPE_STATE 2
#define TYPE_PAYLOAD 3
#define COMMAND_TYPE_FIELD 4
#define STATE_RESPONSE_SIZE_FIELD 5
#define INT_SIZE 4
#define FLOAT_SIZE 8
#define STRING_SIZE -1

int identify_type (cJSON *json)
{
    int i;
    cJSON *subitem = json->child;
    for(i = 0; i < COMMAND_TYPE_FIELD; i++) subitem = subitem->next;
    if(strcmp(subitem->valuestring, "message") == 0) return TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state") == 0) return TYPE_STATE;
    else if(strcmp(subitem->valuestring, "payload") == 0) return TYPE_PAYLOAD;
    else return 0;
}

void translate_message (cJSON *json, FILE *out)
{
    cJSON *subitem;
    
    json = json->child;
    fprintf(out, "    {\n    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        /*%s*/\n", json->valuestring);
    json = json->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    /*Go to the configuration field*/
    json = json->next->next->child;
    /*Go to the response size field using a temporary variable because we will go over other fields.
     *  The original 'json' variable will continue to point in the logical, sequential order*/
    subitem = json->next->next->next;
    fprintf(out, "        .response_size = %d,\n", subitem->valueint);
    fprintf(out, "    },\n    .destination = \"%s\",\n", json->valuestring);
    json = json->next;
    if(json->child) {
        subitem = json->child;
        fprintf(out, "    .origin_groups = {%s", subitem->valuestring);
        while(subitem->next) {
            subitem = subitem->next;
            fprintf(out, ", %s", subitem->valuestring);
        }
        fprintf(out, "},\n");
    } else {
        fprintf(out, "    .origin_groups = NULL,\n");
    }
    json = json->next;
    if(json->child) {
        subitem = json->child;
        fprintf(out, "    .destination_groups = {%s", subitem->valuestring);
        while(subitem->next) {
            subitem = subitem->next;
            fprintf(out, ", %s", subitem->valuestring);
        }
        fprintf(out, "},\n");
    } else {
        fprintf(out, "    .destination_groups = NULL,\n");
    }
    fprintf(out, "    },\n");
}

void translate_state (cJSON *json, FILE *out)
{
    cJSON *subitem;
    
    json = json->child;
    fprintf(out, "    {\n    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        /*%s*/\n", json->valuestring);
    json = json->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    /*Go to the 'config' field*/
    json = json->next->next->child;
    /*Go to 'dimensions' field*/
    json = json->next;
    /*Save the variable 'return_type' and identify it*/
    subitem = json->next;
    if(strcmp(subitem->valuestring, "int") == 0) {
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * INT_SIZE);
    } else if(strcmp(subitem->valuestring, "float") == 0) {
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * FLOAT_SIZE);
    } else {
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * STRING_SIZE);
    }
    /*Go back to 'update_function' field*/
    json = json->prev;
    fprintf(out, "    },\n    .request = %s,\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .dimensions = %d,\n", json->valueint);
    json = json->next->next;
    fprintf(out, "    /*Unit: %s/\n", json->valuestring);
    json = json->next;
    if(!(json->valueint)) fprintf(out, "    /*Dimension_name: NULL*/\n");
    else fprintf(out, "    /*Dimension_name: %s*/\n", json->valuestring);
    json = json->next;
    if(json->child) {
        fprintf(out, "    .expire_group = {\n");
        json = json->child;
        subitem = json->child;
        fprintf(out, "            { .group = %s, .max_espire = %d },\n", subitem->string, 
                subitem->valueint);
        while(json->next) {
            json = json->next;
            subitem = json->child;
            fprintf(out, "            { .group = %s, .max_espire = %d },\n", subitem->string, 
                    subitem->valueint);
        }
        fprintf(out, "        },\n");
    } else {
        fprintf(out, "    .expire_group = NULL,\n");  
    }
    fprintf(out, "    },\n");
}

void translate_payload (cJSON *json, FILE *out)
{
    int i;
    cJSON *subitem;
    
    json = json->child;
    fprintf(out,"    {\n    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        /*%s*/\n", json->valuestring);
    json = json->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    /*Go to configuration field*/
    subitem = json->next->next->child;
    /*Go to response_size field*/
    for(i = 0; i < STATE_RESPONSE_SIZE_FIELD; i++) subitem = subitem->next;
    fprintf(out, "        .response_size = %d,\n", subitem->valueint);
    fprintf(out, "    },\n");
    json = json->next->next->child;
    json = json->next;
    fprintf(out, "    .command = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .parameters = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .arguments = \"%s\",\n", json->valuestring);
    json = json->next;
    if(!(json->valueint)) fprintf(out, "    .data = NULL,\n");
    else fprintf(out, "    .data = \"%s\",\n", json->valuestring);
    fprintf(out, "    },\n");
}

command_list *read_commands (cJSON *json)
{
    int type_id;
    bool first_message = true, first_state = true, first_payload = true;
    command_message *message_list = malloc(sizeof (struct command_state)), *temp_message;
    command_state        *state_list = malloc(sizeof (struct command_state)), *temp_state;
    command_payload  *payload_list = malloc(sizeof (struct command_state)), *temp_payload;
    command_list *commandlist = malloc(sizeof (struct command_list));

    type_id = identify_type(json);

    switch (type_id) {
        case TYPE_MESSAGE:
            temp_message = malloc(sizeof (struct command_message));
            if(temp_message == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_message->msg = json;
            temp_message->next = message_list;
            message_list = temp_message;
            if(first_message) {
                message_list->next = NULL;
                first_message = false;
            }
            break;
        case TYPE_STATE:
            temp_state = malloc(sizeof (struct command_state));
            if(temp_state == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_state->stt = json;
            temp_state->next = state_list;
            state_list = temp_state;
            if(first_state) {
                state_list->next = NULL;
                first_state = false;
            }
            break;
        case TYPE_PAYLOAD:
            temp_payload = malloc(sizeof (struct command_payload));
            if(temp_payload == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_payload->pld = json;
            temp_payload->next = payload_list;
            payload_list = temp_payload;
            if(first_payload) {
                payload_list->next = NULL;
                first_payload = false;
            }
            break;
        default:
            printf("ERROR: Command could not be identified.\n");
            break;
    }
    
    while(json->next) {
        json = json->next;
        type_id = identify_type(json);
        switch (type_id) {
            case TYPE_MESSAGE:
                temp_message = malloc(sizeof (struct command_message));
                if(temp_message == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_message->msg = json;
                temp_message->next = message_list;
                message_list = temp_message;
                if(first_message) {
                    message_list->next = NULL;
                    first_message = false;
                }
                break;
            case TYPE_STATE:
                temp_state = malloc(sizeof (struct command_state));
                if(temp_state == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_state->stt = json;
                temp_state->next = state_list;
                state_list = temp_state;
                if(first_state) {
                    state_list->next = NULL;
                    first_state = false;
                }
                break;
            case TYPE_PAYLOAD:
                temp_payload = malloc(sizeof (struct command_payload));
                if(temp_payload == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_payload->pld = json;
                temp_payload->next = payload_list;
                payload_list = temp_payload;
                if(first_payload) {
                    payload_list->next = NULL;
                    first_payload = false;
                }
                break;
            default:
                printf("ERROR: Command could not be identified.\n");
                break;
        }
    }
    
    commandlist->messages = message_list;
    commandlist->states = state_list;
    commandlist->payloads = payload_list;
    
    return commandlist;
}

void translate_commands(command_list *commandlist) 
{
    FILE *out;
    command_message *message_list;
    command_state *state_list;
    command_payload *payload_list;
    
    message_list = commandlist->messages;
    state_list = commandlist->states;
    payload_list = commandlist->payloads;
    out = fopen("mcs_printed.c", "w");
    
    /*Print all the general header lines*/
    fprintf(out, "/* AUTOGENERATED. DO NOT MODIFY */\n\n"
            "#ifndef __AUTO_MCS_H\n#define __AUTO_MCS_H\n\n"
            "#ifndef __MCS_H\n#error \"This header should not be included "
            "directly. Include mcs.h\"\n#endif\n\n"
            "typedef enum MCSCommand {\n"
            "    MCS_MESSAGE_PROCMAN_START     = 0\n"
            "    MCS_STATE_TEMPERATURE_ARDUINO = 65536 , /* 0 x10000 */\n"
            "    MCS_PAYLOAD_ARDUINO_GET_PIN   = 131072 , /* 0 x20000 */\n"
            "} MCSCommand;\n\n"
        );
    
    /*Print the message header, message(s), and footer*/
    fprintf(out, "const static struct MCSCommandOptionsMessage mcs_command_message_list[] =\n{\n");
    
    translate_message(message_list->msg, out);
    while((message_list->next) != NULL) {
        message_list = message_list->next;
        translate_message(message_list->msg, out);
    }
    
    fprintf(out, "};\n\n#define mcs_command_message_list_size 1\n\n"); /*Must be updated?*/
    
    /*Print the state header, state(s), and footer*/
    fprintf(out, "const static struct MCSCommandOptionsState mcs_command_state_list[] =\n{\n");
    
    translate_state(state_list->stt, out);
    while((state_list->next) != NULL) {
        state_list = state_list->next;
        translate_state(state_list->stt, out);
    }
    
    fprintf(out, "};\n\n#define mcs_command_state_list_size 1\n\n");
    
    /*Print the payload header, payload(s), an footer*/
    fprintf(out, "const static struct MCSCommandOptionsPayload mcs_command_payload_list[] =\n{\n");
    
    translate_payload(payload_list->pld, out);
    while((payload_list->next) != NULL) {
        payload_list = payload_list->next;
        translate_payload(payload_list->pld, out);
    }
    
    fprintf(out, "};\n\n#define mcs_command_payload_list_size 1\n\n#endif");
    
    fclose(out);
}
