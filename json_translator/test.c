/*This is a test file designed to take a single-element json, identify it, and
 print its equivalent in C (commented for building purposes). Next will come
 working with various elements in the json file*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#define TYPE_MESSAGE 1
#define TYPE_STATE 2
#define TYPE_PAYLOAD 3
#define LOCATION_FIELD 4

int identify_type (cJSON *json)
{
    int i;
    cJSON *subitem = json->child;
    for(i = 0; i < LOCATION_FIELD; i++) subitem = subitem->next;
    if(strcmp(subitem->valuestring, "message") == 0) return TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state") == 0) return TYPE_STATE;
    else return TYPE_PAYLOAD;
}

void translate_message (cJSON *json)
{
    FILE *out;
    cJSON *subitem;

    out = fopen("printed.c", "w");
    fprintf(out, "/*---------------------PRINTED FILE---------------------\n\n");
    
    fprintf(out, "const static struct MCSCommandMessage mcs_command_message_list[] =\n{\n    {\n");
    json = json->child;
    fprintf(out, "    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    json = json->next->next->child;
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
    fprintf(out, "    }");
    
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
    json = json->next->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    json = json->next->next->child;
    subitem = json->next->next->next->next;
    /*TO REVISE: What is response size? int/float/string? Or dimension_name?*/
    fprintf(out, "        .response_size = %d\n", subitem->valueint);
    fprintf(out, "    },\n    .request = %s,\n", json->valuestring);
    json = json->next;
    fprintf(out, "    .dimensions = %d,\n", json->valueint);
    /*TO REVISE: 'Unit' is not used? Same with previous others*/
    json = json->next->next->next->next;
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
    FILE *out;
    cJSON *subitem;

    out = fopen("printed.c", "w");
    fprintf(out, "/*---------------------PRINTED FILE--------------------\n\n");
    
    fprintf(out, "const static struct MCSCommandPayload mcs_command_payload_"
            "list[] =\n{\n    {\n");
    json = json->child;
    fprintf(out,"    .cmd = {\n        .name = \"%s\",\n", json->valuestring);
    json = json->next->next;
    fprintf(out, "        .nargs = %d,\n", json->valueint);
    json = json->next;
    if(json->valueint) fprintf(out, "        .raw_data = true,\n");
    else fprintf(out, "        .raw_data = false,\n");
    subitem = json->next->next->child;
    subitem = subitem->next->next->next->next->next;
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
    /*TO REVISE: Rule out this field*/
    else fprintf(out, "    .data = /*...*/,\n");
    fprintf(out, "    },\n};");
    
    fprintf(out, "\n*/");
    fclose(out);
}

void main (){
    int type_id; 
    FILE *f; 
    long len; 
    char *data; 
    cJSON *json;

    /*For running and building purposes*/
    printf("-------------------------Start main-------------------------\n");
    
    /*Load the json file, to choose from 'single_message', 'single_state' and
     'single_payload' depending on what to test. Keep in mind that everything
     will printed and thus overwritten in the same output file.*/
    f = fopen("single_message.json", "r");
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
        printf("|| First, identify the type of command!\n");
        type_id = identify_type (json);
        printf("|| Finished idenifying command. Result identifier: %d\n", type_id);
        printf("|| Printing translated message\n");
        
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
        }
        printf("|| Finished printing translated message\n");
    }
    
    cJSON_Delete(json);
    free(data);
    
    printf("\n--------------------------End main--------------------------\n");
}