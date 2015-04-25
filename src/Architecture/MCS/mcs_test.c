/*This file is designed to test the functions declared in 'mcs.c', mainly to open a .json file,
 read the commands and place them to buffers according to their type, and then print their
 translation to 'printed.c'*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "mcs.h"

void main() {
    FILE *f; 
    long len; 
    char *data; 
    cJSON *json;
    command_list *commandlist;

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


