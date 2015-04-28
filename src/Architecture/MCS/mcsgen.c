#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cJSON.h"
#include "mcsgen.h"
#include "../include/mcs.h"

int identify_type(cJSON *json)
{
    int i;
    cJSON *subitem = json->child;
    for(i = 0; i < COMMAND_TYPE_FIELD; i++) subitem = subitem->next;
    if(strcmp(subitem->valuestring, "message") == 0) return MCS_TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state") == 0) return MCS_TYPE_STATE;
    else if(strcmp(subitem->valuestring, "payload") == 0) return MCS_TYPE_PAYLOAD;
    else return 0;
}

void translate_message(cJSON *json, FILE *out)
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

void translate_state(cJSON *json, FILE *out)
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

void translate_payload(cJSON *json, FILE *out)
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

MCSGenCommandList *read_commands(cJSON *json)
{
    int type_id;
    bool first_message = true, first_state = true, first_payload = true;
    MCSGenCommand *message_list = malloc(sizeof (struct MCSGenCommand)), *temp_message,
                  *state_list = malloc(sizeof (struct MCSGenCommand)), *temp_state,
                  *payload_list = malloc(sizeof (struct MCSGenCommand)), *temp_payload;
    MCSGenCommandList *command_list = malloc(sizeof (struct MCSGenCommandList));

    type_id = identify_type(json);

    switch (type_id) {
        case MCS_TYPE_MESSAGE:
            temp_message = malloc(sizeof (struct MCSGenCommand));
            if(temp_message == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_message->cmd = json;
            temp_message->next = message_list;
            message_list = temp_message;
            if(first_message) {
                message_list->next = NULL;
                first_message = false;
            }
            break;
        case MCS_TYPE_STATE:
            temp_state = malloc(sizeof (struct MCSGenCommand));
            if(temp_state == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_state->cmd = json;
            temp_state->next = state_list;
            state_list = temp_state;
            if(first_state) {
                state_list->next = NULL;
                first_state = false;
            }
            break;
        case MCS_TYPE_PAYLOAD:
            temp_payload = malloc(sizeof (struct MCSGenCommand));
            if(temp_payload == NULL) printf("ERROR: Could not allocate memory.\n");
            temp_payload->cmd = json;
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
            case MCS_TYPE_MESSAGE:
                temp_message = malloc(sizeof (struct MCSGenCommand));
                if(temp_message == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_message->cmd = json;
                temp_message->next = message_list;
                message_list = temp_message;
                if(first_message) {
                    message_list->next = NULL;
                    first_message = false;
                }
                break;
            case MCS_TYPE_STATE:
                temp_state = malloc(sizeof (struct MCSGenCommand));
                if(temp_state == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_state->cmd = json;
                temp_state->next = state_list;
                state_list = temp_state;
                if(first_state) {
                    state_list->next = NULL;
                    first_state = false;
                }
                break;
            case MCS_TYPE_PAYLOAD:
                temp_payload = malloc(sizeof (struct MCSGenCommand));
                if(temp_payload == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_payload->cmd = json;
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
    
    command_list->messages = message_list;
    command_list->states = state_list;
    command_list->payloads = payload_list;
    
    return command_list;
}

void translate_commands(MCSGenCommandList *command_list) 
{
    int message_count = 1, state_count = 1, payload_count = 1, command_count = 0, type_value;
    FILE *out;
    MCSGenCommand *message_list, *state_list, *payload_list, *temp;
    
    message_list = command_list->messages;
    state_list = command_list->states;
    payload_list = command_list->payloads;
    out = fopen("mcs_printed.c", "w");
    
    /*Print all the general header lines*/
    fprintf(out, "/* AUTOGENERATED. DO NOT MODIFY */\n\n"
            "#ifndef __AUTO_MCS_H\n#define __AUTO_MCS_H\n\n"
            "#ifndef __MCS_H\n#error \"This header should not be included "
            "directly. Include mcs.h\"\n#endif\n\n"
        );
    
    /*Print the enum list*/
    fprintf(out, "typedef enum MCSCommand {\n");
    
    temp = message_list;
    type_value = TYPE_MESSAGE_DEC;
    translate_enum(temp->cmd, out, command_count, type_value);
    while(temp->next != NULL){
        command_count++;
        temp = temp->next;
        translate_enum(temp->cmd, out, command_count, type_value);
    }
    
    temp = state_list;
    type_value = TYPE_STATE_DEC;
    command_count++;
    translate_enum(temp->cmd, out, command_count, type_value);
    while(temp->next != NULL){
        command_count++;
        temp = temp->next;
        translate_enum(temp->cmd, out, command_count, type_value);
    }
    
    temp = payload_list;
    type_value = TYPE_PAYLOAD_DEC;
    command_count++;
    translate_enum(temp->cmd, out, command_count, type_value);
    while(temp->next != NULL){
        command_count++;
        temp = temp->next;
        translate_enum(temp->cmd, out, command_count, type_value);
    }
    
    fprintf(out, "} MCSCommand;\n\n");
    
    /*Print the message header, message(s), and footer*/
    fprintf(out, "const static struct MCSCommandOptionsMessage mcs_command_message_list[] =\n{\n");
    
    translate_message(message_list->cmd, out);
    while((message_list->next) != NULL) {
        message_list = message_list->next;
        translate_message(message_list->cmd, out);
        message_count++;
    }
    
    fprintf(out, "};\n\n#define mcs_command_message_list_size %d\n\n", message_count);
    
    /*Print the state header, state(s), and footer*/
    fprintf(out, "const static struct MCSCommandOptionsState mcs_command_state_list[] =\n{\n");
    
    translate_state(state_list->cmd, out);
    while((state_list->next) != NULL) {
        state_list = state_list->next;
        translate_state(state_list->cmd, out);
        state_count++;
    }
    
    fprintf(out, "};\n\n#define mcs_command_state_list_size %d\n\n", state_count);
    
    /*Print the payload header, payload(s), an footer*/
    fprintf(out, "const static struct MCSCommandOptionsPayload mcs_command_payload_list[] =\n{\n");
    
    translate_payload(payload_list->cmd, out);
    while((payload_list->next) != NULL) {
        payload_list = payload_list->next;
        translate_payload(payload_list->cmd, out);
        payload_count++;
    }
    
    fprintf(out, "};\n\n#define mcs_command_payload_list_size %d\n\n#endif", payload_count);
    
    fclose(out);
}

void translate_enum(cJSON *json, FILE *out, int command_count, int type_value)
{
    int i = 0, dec_message = 0, value;
    json = json->child;
    
    switch(type_value){
        case TYPE_MESSAGE_DEC:
            fprintf(out, "    MCS_MESSAGE_");
            break;
        case TYPE_STATE_DEC:
            fprintf(out, "    MCS_STATE_");
            break;
        case TYPE_PAYLOAD_DEC:
            fprintf(out, "    MCS_PAYLOAD_");
            break;
    }
    
    while (json->valuestring[i]) {
        fprintf(out, "%c", toupper(json->valuestring[i]));
        i++;
    }
    value = type_value + command_count;
    fprintf(out, " = %d,\n", value);    
}

void main() {
    FILE *f; 
    long len; 
    char *data; 
    cJSON *json;
    MCSGenCommandList *commandlist;

    /*Open the json file*/
    f = fopen("mcs_json_commands.json", "r");
    fseek(f, 0, SEEK_END); 
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = malloc(len + 1);
    fread(data, 1, len, f); 
    fclose(f);
    
    /*Parse json and identify its type before translating*/
    json = cJSON_Parse(data);
    if(!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        /*Go to the first json element*/
        json = json->child->child;
        
        /*Place individual commands in their respective buffers*/
        commandlist = read_commands(json);
        
        /*Tanslate and print to 'printed.c'*/
        translate_commands(commandlist);
    }
    
    cJSON_Delete(json);
    free(data);
}