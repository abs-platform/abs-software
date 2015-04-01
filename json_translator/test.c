/*This is a test file to start working on and identify the parsed c structures.
 Taking the functions from cJSON's library tests as an example, this will read
 a simple json structured file, identify its parameters and print them on screen
 with their correct labels*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

#define TYPE_MESSAGE 1
#define TYPE_STATE 2
#define TYPE_PAYLOAD 3
#define TYPE_LOCATION 4

int identify_type (cJSON *json){
    int i;
    cJSON *subitem = json->child;
    for(i = 0; i < TYPE_LOCATION; i++) subitem = subitem->next;
    i = printf("%s\n", subitem->valuestring);
    if(strcmp(subitem->valuestring, "message") == 0) return TYPE_MESSAGE;
    else if(strcmp(subitem->valuestring, "state")) return TYPE_STATE;
    else return TYPE_PAYLOAD;
}

void translate_message (cJSON *json){
    //...
    FILE *ofp;
    ofp = fopen("printed.c", "w");
    fprintf(ofp, "//Hello World");
    fclose (ofp);
}

void translate_state (cJSON *json){
    //...
}

void translate_payload (cJSON *json){
    //...
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
    f = fopen("mcs_conf_file.json", "rb"); /*L.17 "b" in opening mode makes no effect!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    fseek(f, 0, SEEK_END); 
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*)malloc(len + 1); /*L.20 It is not necessary to cast the result of a malloc*/
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
    
    free(data);
    
    a = printf("\nEnd main===================================================\n");
}