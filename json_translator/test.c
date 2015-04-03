/*This is a test file designed to take a single-element json, identify it, and
 print its equivalent in C (commented for building purposes). Next will come
 working with various elements in the json file*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#define TYPE_MESSAGE 1
#define TYPE_STATE 2
#define TYPE_PAYLOAD 3
#define TYPE_LOCATION 4

int identify_type (cJSON *json){
    int i;
    int res;
    cJSON *subitem = json->child;
    for(i = 0; i < TYPE_LOCATION; i++) subitem = subitem->next;
    i = printf("%s\n", subitem->valuestring);
    if(strcmp(subitem->valuestring, "message") == 0) return TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state")) return TYPE_STATE;
    else return TYPE_PAYLOAD;
}

void translate_message (cJSON *json){
    FILE *out;
    cJSON *subitem;

    out = fopen("printed.c", "w");
    fprintf(out, "/*HERE BEGINS THE PRINTED FILE, commented for building p\n\n");
    
    fprintf(out, "const static struct MCSCommandMessage "
            "mcs_command_message_list[] =\n{\n    {\n");
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
    if(json->child){
        subitem = json->child;
        fprintf(out, "    .origin_groups = {%s", subitem->valuestring);
        while(subitem->next){
            subitem = subitem->next;
            fprintf(out, ", %s", subitem->valuestring);
        }
        fprintf(out, "},\n");
    } else {
        fprintf(out, "    .origin_groups = NULL,\n");
    }
    json = json->next;
    if (json->child) {
        subitem = json->child;
        fprintf(out, "    .destination_groups = {%s", subitem->valuestring);
        while (subitem->next) {
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

void translate_state (cJSON *json){
    /*...*/
}

void translate_payload (cJSON *json){
    /*...*/
}

void main (){
    int a, type_id; 
    FILE *f; 
    long len; 
    char *data; 
    cJSON *json;

    /*For running and building purposes*/
    a = printf("Start main\n");
    
    /*Load the .json file to char*/
    f = fopen("mcs_conf_file.json", "rb"); /*CORRECT: L.17 "b" in opening mode makes no effect*/
    fseek(f, 0, SEEK_END); 
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*)malloc(len + 1); /*CORRECT: L.20 It is not necessary to cast the result of a malloc*/
    fread(data, 1, len, f); 
    fclose(f);
    
    /*Parse json and operate*/
    json = cJSON_Parse(data);
    if(!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        a = printf("Start function\n");
        type_id = identify_type (json);
        a = printf("End function: %d\n", type_id);
        
        switch (type_id) {
            case TYPE_MESSAGE:
                translate_message(json);
            case TYPE_STATE:
                translate_state(json);
            case TYPE_PAYLOAD:
                translate_payload(json);
        }
        
    }
    
    cJSON_Delete(json);
    free(data);
    
    a = printf("\nEnd main\n");
}