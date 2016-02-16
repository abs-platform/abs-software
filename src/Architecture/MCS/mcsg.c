#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"
#include "mcsg.h"
#include "mcs.h"

static char *str_to_upper(const char *str)
{
    size_t i;
    char *str_out = malloc(strlen(str) + 1);
    for(i = 0; str[i]; i++) {
        str_out[i] = toupper(str[i]);
    }

    str_out[i] = 0; /* NULL character */

    return str_out;
}

int mcsg_type_identifier(cJSON *json)
{
    int i;
    cJSON *subitem = json->child;
    for(i = 0; i < MCSG_COMMAND_TYPE_FIELD; i++) subitem = subitem->next;
    if(strcmp(subitem->valuestring, "message") == 0) return MCS_TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state") == 0) return MCS_TYPE_STATE;
    else if(strcmp(subitem->valuestring, "payload") == 0) return MCS_TYPE_PAYLOAD;
    else return 0;
}

void mcsg_message_translator(cJSON *json, FILE *out)
{
    cJSON *subitem;
    char *group_name;
    
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
    fprintf(out, "    .origin_groups = {");
    if(json->child) {
        subitem = json->child;
        group_name = str_to_upper(subitem->valuestring);
        fprintf(out, "SDB_GROUP_%s", group_name);
        free(group_name);
        while(subitem->next) {
            group_name = str_to_upper(subitem->valuestring);
            subitem = subitem->next;
            fprintf(out, ", SDB_GROUP_%s", group_name);
            free(group_name);
        }
    }
    fprintf(out, "},\n");
    json = json->next;
    fprintf(out, "    .destination_groups = {");
    if(json->child) {
        subitem = json->child;
        group_name = str_to_upper(subitem->valuestring);
        fprintf(out, "SDB_GROUP_%s", group_name);
        free(group_name);
        while(subitem->next) {
            subitem = subitem->next;
            group_name = str_to_upper(subitem->valuestring);
            fprintf(out, ", SDB_GROUP_%s", group_name);
            free(group_name);
        }
    }
    fprintf(out, "},\n");
    fprintf(out, "    },\n");
}

void mcsg_state_translator(cJSON *json, FILE *out)
{
    cJSON *subitem;
    char *group_name;
    
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
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * MCSG_INT_SIZE);
    } else if(strcmp(subitem->valuestring, "float") == 0) {
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * MCSG_FLOAT_SIZE);
    } else {
        fprintf(out, "        .response_size = %d,\n", (json->valueint) * MCSG_STRING_SIZE);
    }
    /*Go back to 'update_function' field*/
    json = json->prev;
    fprintf(out, "    },\n    .request = %s,\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .dimensions = %d,\n", json->valueint);
    json = json->next->next;
    fprintf(out, "    /*Unit: %s*/\n", json->valuestring);
    json = json->next;
    if(!(json->valueint)) fprintf(out, "    /*Dimension_name: NULL*/\n");
    else fprintf(out, "    /*Dimension_name: %s*/\n", json->valuestring);
    json = json->next;
    if(json->child) {
        fprintf(out, "    .expire_group = {\n");
        json = json->child;
        subitem = json->child;
        group_name = str_to_upper(subitem->string);
        fprintf(out, "            { .group = SDB_GROUP_%s, .max_expire = %d },\n", group_name, 
                subitem->valueint);
        free(group_name);
        while(json->next) {
            json = json->next;
            subitem = json->child;
            group_name = str_to_upper(subitem->string);
            fprintf(out, "            { .group = SDB_GROUP_%s, .max_expire = %d },\n", group_name, 
                    subitem->valueint);
            free(group_name);
        }
        fprintf(out, "        },\n");
    } else {
        fprintf(out, "    .expire_group = NULL,\n");  
    }
    fprintf(out, "    },\n");
}

void mcsg_payload_translator(cJSON *json, FILE *out)
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
    json = json->next->next->child;
    /*Go to response_size field*/
    subitem = json;
    for(i = 0; i < MCSG_STATE_RESPONSE_SIZE_FIELD; i++) subitem = subitem->next;
    fprintf(out, "        .response_size = %d,\n", subitem->valueint);
    fprintf(out, "    },\n");
    fprintf(out, "    .command = %d,\n", json->valueint);
    json = json->next;
    fprintf(out, "    .parameters = %d,\n", json->valueint);
    json = json->next;
    fprintf(out, "    .arguments = \"%s\",\n", json->valuestring);
    json = json->next;
    if(!(json->valueint)) fprintf(out, "    .data = NULL,\n"); /*How is this field when not null?*/
    else fprintf(out, "    .data = \"%s\",\n", json->valuestring);
    fprintf(out, "    },\n");
}

MCSGCommandList *mcsg_commands_reader(cJSON *json)
{
    int type_id, command_count;
    char *command_name, *prefix;
    bool first_message = true, first_state = true, first_payload = true, first_enum = true;
    MCSGCommand *message_list, *temp_message;
    MCSGCommand *state_list, *temp_state;
    MCSGCommand *payload_list, *temp_payload;
    MCSGCommandList *command_list = malloc(sizeof(struct MCSGCommandList));
    if(command_list == NULL) printf("ERROR: Could not allocate memory.\n");
    MCSGEnumList *enum_list, *temp_enum;
    
    message_list = malloc(sizeof(struct MCSGCommand));
    if(message_list == NULL) printf("ERROR: Could not allocate memory.\n");
    state_list = malloc(sizeof(struct MCSGCommand));
    if(state_list == NULL) printf("ERROR: Could not allocate memory.\n");
    payload_list = malloc(sizeof(struct MCSGCommand));
    if(payload_list == NULL) printf("ERROR: Could not allocate memory.\n");
    enum_list = malloc(sizeof(struct MCSGEnumList));
    if(enum_list == NULL) printf("ERROR: Could not allocate memory.\n");
    
    json = json->child->child;
    command_count = 0;
    
    do {
        type_id = mcsg_type_identifier(json);

        switch (type_id) {
            case MCS_TYPE_MESSAGE:
                /*Save to buffer*/
                temp_message = malloc(sizeof(struct MCSGCommand));
                if(temp_message == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_message->cmd = json;
                temp_message->next = message_list;
                message_list = temp_message;
                if(first_message) {
                    message_list->next = NULL;
                    first_message = false;
                }
                
                /*Index to enum list*/
                prefix = "MCS_MESSAGE_";
                temp_enum = malloc(sizeof(struct MCSGEnumList));
                command_name = str_to_upper(json->child->valuestring);
                temp_enum->name = malloc(strlen(command_name)+strlen(prefix));
                strcpy(temp_enum->name, prefix);
                strcat(temp_enum->name, command_name);
                temp_enum->value = MCSG_TYPE_MESSAGE_DEC + command_count;
                command_count++;
                temp_enum->next = enum_list;
                enum_list = temp_enum;
                if(first_enum) {
                    enum_list->next = NULL;
                    first_enum = false;
                }
                break;
            case MCS_TYPE_STATE:
                /*Save to buffer*/
                temp_state = malloc(sizeof(struct MCSGCommand));
                if(temp_state == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_state->cmd = json;
                temp_state->next = state_list;
                state_list = temp_state;
                if(first_state) {
                    state_list->next = NULL;
                    first_state = false;
                }
                
                /*Index to enum list*/
                prefix = "MCS_STATE_";
                temp_enum = malloc(sizeof(struct MCSGEnumList));
                command_name = str_to_upper(json->child->valuestring);
                temp_enum->name = malloc(strlen(command_name)+strlen(prefix)+1);
                strcpy(temp_enum->name, prefix);
                strcat(temp_enum->name, command_name);
                temp_enum->value = MCSG_TYPE_STATE_DEC + command_count;
                command_count++;
                temp_enum->next = enum_list;
                enum_list = temp_enum;
                if (first_enum) {
                    enum_list->next = NULL;
                    first_enum = false;
                }
                break;
            case MCS_TYPE_PAYLOAD:
                /*Save to buffer*/
                temp_payload = malloc(sizeof(struct MCSGCommand));
                if(temp_payload == NULL) printf("ERROR: Could not allocate memory.\n");
                temp_payload->cmd = json;
                temp_payload->next = payload_list;
                payload_list = temp_payload;
                if(first_payload) {
                    payload_list->next = NULL;
                    first_payload = false;
                }
                
                /*Index to enum list*/
                prefix = "MCS_PAYLOAD_";
                temp_enum = malloc(sizeof(struct MCSGEnumList));
                command_name = str_to_upper(json->child->valuestring);
                temp_enum->name = malloc(strlen(command_name)+strlen(prefix)+1);
                strcpy(temp_enum->name, prefix);
                strcat(temp_enum->name, command_name);
                temp_enum->value = MCSG_TYPE_PAYLOAD_DEC + command_count;
                command_count++;
                temp_enum->next = enum_list;
                enum_list = temp_enum;
                if(first_enum) {
                    enum_list->next = NULL;
                    first_enum = false;
                }
                break;
            default:
                printf("ERROR: Command could not be identified.\n");
                break;
        }
        json = json->next;

        free(command_name);
    } while(json);
    
    command_list->messages = message_list;
    command_list->states = state_list;
    command_list->payloads = payload_list;
    command_list->enums = enum_list;
    
    return command_list;
}

void mcsg_commands_translator(MCSGCommandList *command_list, FILE *out) 
{
    int command_list_size;
    MCSGCommand *message_list, *state_list, *payload_list;
    MCSGEnumList *enum_list;
    
    message_list = command_list->messages;
    state_list = command_list->states;
    payload_list = command_list->payloads;
    enum_list = command_list->enums;
    
    /*Print all the general header lines*/
    fprintf(out, "/* AUTOGENERATED. DO NOT MODIFY */\n\n"
            "#ifndef __AUTO_MCS_H\n#define __AUTO_MCS_H\n\n"
            "#ifndef __MCS_H\n#error \"This header should not be included "
            "directly. Include mcs.h\"\n#endif\n\n"
        );
    
    /*Print the enum list*/
    fprintf(out, "typedef enum MCSCommand {\n");
    
    do {
        mcsg_enum_translator(enum_list, out);
        enum_list = enum_list->next;
    } while(enum_list);
    
    fprintf(out, "} MCSCommand;\n\n");
    
    /*Print the message header, message(s), and footer*/
    fprintf(out, "static const struct MCSCommandOptionsMessage mcs_command_message_list[] =\n{\n");
    
    mcsg_message_translator(message_list->cmd, out);
    command_list_size = 1;
    while((message_list->next) != NULL) {
        message_list = message_list->next;
        mcsg_message_translator(message_list->cmd, out);
        command_list_size++;
    }
    
    
    fprintf(out, "};\n\n#define MCS_COMMAND_MESSAGE_LIST_SIZE %d\n\n", command_list_size);
    /*Print the state header, state(s), and footer*/
    fprintf(out, "static const struct MCSCommandOptionsState mcs_command_state_list[] =\n{\n");
    
    mcsg_state_translator(state_list->cmd, out);
    command_list_size = 1;
    while((state_list->next) != NULL) {
        state_list = state_list->next;
        mcsg_state_translator(state_list->cmd, out);
        command_list_size++;
    }
    
    
    fprintf(out, "};\n\n#define MCS_COMMAND_STATE_LIST_SIZE %d\n\n", command_list_size);
    /*Print the payload header, payload(s), an footer*/
    fprintf(out, "static const struct MCSCommandOptionsPayload mcs_command_payload_list[] =\n{\n");
    
    mcsg_payload_translator(payload_list->cmd, out);
    command_list_size = 1;
    while((payload_list->next) != NULL) {
        payload_list = payload_list->next;
        mcsg_payload_translator(payload_list->cmd, out);
        command_list_size++;
    }
    
    fprintf(out, "};\n\n#define MCS_COMMAND_PAYLOAD_LIST_SIZE %d\n\n#endif", command_list_size);
}

void mcsg_enum_translator(MCSGEnumList *enum_list, FILE *out)
{
    fprintf(out, "    %s = %d,\n", enum_list->name, enum_list->value);
}

void mcsg_java_translator(MCSGEnumList *enum_list, FILE *out)
{
    size_t i;
    int understroke;
    /*Print the java headers*/
    fprintf(out, "public class SDBPacket {\n\n    public enum CMD {\n\n");
    
    /*Print the enum list*/
    do {
        /*Cut out the prefix from the names*/
        understroke = 0;
        fprintf(out, "        ");
        for(i = 0; i < strlen(enum_list->name); i++){
            if (understroke >= 2) fprintf(out, "%c", enum_list->name[i]);
            if (enum_list->name[i] == '_') understroke++;
        }
        fprintf(out, " (%d)", enum_list->value);
        enum_list = enum_list->next;
        if (enum_list) fprintf(out, ",\n");
        else fprintf(out, ";\n");
    } while (enum_list);
    
    fprintf(out, "\n        private int cmd;\n\n        private CMD (int cmd)\n"
                "        {\n            this.cmd = cmd;\n        }\n\n"
                "        public int getCmd ()\n        {\n            return cmd;\n"
                "        }\n    }\n\n    private CMD cmd;\n    private byte [] args;\n"
                "    private byte [] data;\n\n    public SDBPacket (CMD cmd, byte [] "
                "args, byte [] data)\n    {\n        //...\n    }\n\n}");
    
}

int main() {
    FILE *f, *out; 
    long len; 
    char *data; 
    cJSON *json;
    MCSGCommandList *commandlist;

    /*Open the json file*/
    f = fopen("mcsg.json", "r");
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
        /*Place individual commands in their respective buffers*/
        commandlist = mcsg_commands_reader(json);
        
        /*Translate and print to 'printed.c'*/
        out = fopen("auto_mcs.h", "w");
        mcsg_commands_translator(commandlist, out);
        fclose(out);
        
        /*Translate to java*/
        out = fopen("mcsg_java_printed.java", "w");
        mcsg_java_translator(commandlist->enums, out);
        fclose(out);
        
        
    }
    
    cJSON_Delete(json);
    free(data);

    return 0;
}
