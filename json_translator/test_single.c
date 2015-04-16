/*This is a test file designed to take a single-element json, identify it, and
 print its equivalent in C (commented for building purposes). Next will come
 working with various elements in the json file*/

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

void translate_message (cJSON *json)
{
    FILE *out;
    cJSON *subitem;

    out = fopen("printed.c", "w");
    fprintf(out, "/*---------------------PRINTED FILE--------------------\n\n");
    
    fprintf(out, "const static struct MCSCommandMessage "
            "mcs_command_message_list[] =\n{\n    {\n");
    json = json->child;
    fprintf(out, "    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        //%s\n", json->valuestring);
    json = json->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    /*Go to the configuration field*/
    json = json->next->next->child;
    /*Go to the response size field using a temporary variable because we will
     go over other fields. The original 'json' variable will continue to point
     in the logical, sequential order*/
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
    fprintf(out, "};");
    
    fprintf(out, "\n*/");
    fclose (out);
}

void translate_state (cJSON *json)
{
    FILE *out;
    cJSON *subitem;
    
    out = fopen("printed.c", "w");
    fprintf(out, "/*---------------------PRINTED FILE--------------------\n\n");
    
    fprintf(out, "const static struct MCSCommandState mcs_command_state_list[]"
            " =\n{\n    {\n");
    json = json->child;
    fprintf(out, "    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        //%s\n", json->valuestring);
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
        fprintf(out, "        .response_size = %d,\n", (json->valueint)*INT_SIZE);
    } else if(strcmp(subitem->valuestring, "float") == 0) {
        fprintf(out, "        .response_size = %d,\n", (json->valueint)*FLOAT_SIZE);
    } else {
        fprintf(out, "        .response_size = %d,\n", (json->valueint)*STRING_SIZE);
    }
    /*Go back to 'update_function' field*/
    json = json->prev;
    fprintf(out, "    },\n    .request = %s,\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .dimensions = %d,\n", json->valueint);
    json = json->next->next;
    fprintf(out, "    //Unit: %s\n", json->valuestring);
    json = json->next;
    if(!(json->valueint)) fprintf(out, "    //Dimension_name: NULL\n");
    else fprintf(out, "    //Dimension_name: %s\n", json->valuestring);
    json = json->next;
    if(json->child) {
        fprintf(out, "    .expire_group = {\n");
        json = json->child;
        subitem = json->child;
        fprintf(out, "            { .group = %s, .max_espire = %d },\n", 
                subitem->string, subitem->valueint);
        while(json->next) {
            json = json->next;
            subitem = json->child;
            fprintf(out, "            { .group = %s, .max_espire = %d },\n", 
                    subitem->string, subitem->valueint);
        }       
    } else {
        fprintf(out, "    .expire_group = NULL,\n");  
    }
    fprintf(out, "    },\n};");
    
    fprintf(out, "\n*/");
    fclose(out);
}

void translate_payload (cJSON *json)
{
    int i;
    FILE *out;
    cJSON *subitem;

    out = fopen("printed.c", "w");
    fprintf(out, "/*---------------------PRINTED FILE--------------------\n\n");
    
    fprintf(out, "const static struct MCSCommandPayload mcs_command_payload_"
            "list[] =\n{\n    {\n");
    json = json->child;
    fprintf(out,"    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next;
    fprintf(out, "        //%s\n", json->valuestring);
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
    fprintf(out, "    },\n};");
    
    fprintf(out, "\n*/");
    fclose(out);
}

void main_nope (){
    int type_id; 
    FILE *f; 
    long len; 
    char *data; 
    cJSON *json;
    
    /*Load the json file, to choose from 'single_message', 'single_state' and
     'single_payload' depending on what to test. Keep in mind that everything
     will printed and thus overwritten in the same output file.*/
    f = fopen("single_state.json", "r");
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
        type_id = identify_type (json);
        
        switch (type_id) {
            case TYPE_MESSAGE:
                translate_message(json);
                break;
            case TYPE_STATE:
                translate_state(json);
                break;
            case TYPE_PAYLOAD:
                translate_payload(json);
                break;
            default:
                printf("ERROR: Command couldn't be identified.\n");
                break;
        }
    }
    
    cJSON_Delete(json);
    free(data);
}