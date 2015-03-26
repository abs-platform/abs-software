/*This is a test file to start working on and identify the parsed c structures.
 Taking the functions from cJSON's library tests as an example, this will read
 a simple json structured file, identify its parameters and print them on screen
 with their correct labels*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void main (){
    int a; FILE *f; long len; char *data; cJSON *json;

    /*For running and building purposes*/
    a = printf("Start main\n");
    
    /*Load the .json file to char*/
    f = fopen("mcs_conf_file.json", "rb"); fseek(f, 0, SEEK_END); 
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = (char*)malloc(len+1); fread(data, 1, len, f); fclose(f);
    
    /*Parse json and operate*/
    json = cJSON_Parse(data);
    if(!json) {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else {
        /*Prints the first element and then looks for child, or sibling*/
        json = json->child;
        a = printf("First element: %s", json->string);
        
        while(json->child){
            json = json->child;
            a = printf("\n    Child: %s", json->valuestring);
        }
        
        while(json->next){
            json = json->next;
            a = printf("\n    Sibling: %s", json->valuestring);
        }
        cJSON_Delete(json);
    }
    
    free(data);
    
    a = printf("\nEnd main");
}