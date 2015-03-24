#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void read_text(char *text)
{
	char *out; cJSON *json;

	json = cJSON_Parse(text);
	if (!json) {printf("Error before: [%s]\n", cJSON_GetErrorPtr());}
	else
	{
		out = cJSON_Print(json);
		cJSON_Delete(json);
		printf("%s\n", out);
		free(out);
	}
}

void read_file(char *filename)
{
	FILE *f = fopen(filename, "rb"); fseek(f, 0, SEEK_END); long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *data = (char*)malloc(len+1); fread(data, 1, len, f); fclose(f);
	read_text(data);
	free(data);
}

void main (){
    //Read a json structure file
    read_file("jt.json");
}
