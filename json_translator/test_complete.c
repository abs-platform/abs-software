/*Test for the complete json structure example*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

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
    
    fprintf(out, "const static struct MCSCommandOptionsMessage mcs_command_message_list[] =\n{\n"
            "    {\n");
    json = json->child;
    fprintf(out, "    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
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
    fprintf(out, "    }\n");
    fprintf(out, "};\n\n");
    fprintf(out, "#define mcs_command_message_list_size 1\n\n");
}

void translate_state (cJSON *json, FILE *out)
{
    cJSON *subitem;
    
    fprintf(out, "const static struct MCSCommandOptionsState mcs_command_state_list[] =\n{\n"
            "    {\n");
    json = json->child;
    fprintf(out, "    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
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
    } else {
        fprintf(out, "    .expire_group = NULL,\n");  
    }
    fprintf(out, "    },\n};\n\n");
    fprintf(out, "#define mcs_command_state_list_size 1\n\n");
}

void translate_payload (cJSON *json, FILE *out)
{
    int i;
    cJSON *subitem;
    
    fprintf(out, "const static struct MCSCommandOptionsPayload mcs_command_payload_list[] =\n{\n"
            "    {\n");
    json = json->child;
    fprintf(out,"    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
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
    fprintf(out, "    },\n};\n\n");
    
    fprintf(out, "#define mcs_command_payload_list_size 1\n\n#endif");
}

void main() {
    int type_id, i; 
    FILE *f, *out; 
    long len; 
    char *data; 
    cJSON *json, *single_command;

    /*For running and building purposes*/
    printf("-------------------------Start main-------------------------\n");
    
    f = fopen("json_example_complete.json", "r");
    fseek(f, 0, SEEK_END); 
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*)malloc(len + 1);
    fread(data, 1, len, f); 
    fclose(f);
    
    /*Parse json and identify its type before translating*/
    json = cJSON_Parse(data);
    if(!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        out = fopen("printed.c", "w");
        json = json->child->child;
        
        /*Print all he headers*/
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
        
        /*Access the first single command from the array*/
        single_command = json->child;
        printf("|| First, identify the command!\n");
        type_id = identify_type(json);
        printf("|| Identified. Type is: %d\n", type_id);
        switch (type_id) {
            case TYPE_MESSAGE:
                translate_message(json, out);
                break;
            case TYPE_STATE:
                translate_state(json, out);
                break;
            case TYPE_PAYLOAD:
                translate_payload(json, out);
                break;
        }
        /*Access and translate its siblings (if any)*/
        while (json->next){
            json = json->next;
            single_command = json->child;
            printf("|| First, identify the command!\n");
            type_id = identify_type(json);
            printf("|| Identified. Type is: %d\n", type_id);
            switch(type_id) {
                case TYPE_MESSAGE:
                    translate_message(json, out);
                    break;
                case TYPE_STATE:
                    translate_state(json, out);
                    break;
                case TYPE_PAYLOAD:
                    translate_payload(json, out);
                    break;
            }
        }
        fclose(out);
    }
    
    cJSON_Delete(json);
    free(data);
    
    printf("\n--------------------------End main--------------------------\n");
}

