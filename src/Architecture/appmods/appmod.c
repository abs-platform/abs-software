#include <stdio.h>
#include <stdlib.h>

#include "appmod.h"
#include "assets.h"

#define dbName "permissions.db"

/* gcc appmod.c assets.c JUnzip/junzip.c sqlite/sqlite3.c -lz -DDEBUG=1 -o out */

int init(sqlite3 *db, char *filename)
{
    db = openDb(dbName);
    int res = createPermissionsTable(db);
	return res;
}

int setup(sqlite3 *db, char *filename)
{
	/* does nothing */
	return 0;
}

int run(sqlite3 *db, char *filename)
{
	int ii, res, size;
	unsigned char *data;   
    size = getManifestFromAPK(filename, &data);
    char **matches = getPermissionsFromManifest(data, size, &res);
    for(ii = 0; ii < res; ii++) {
        printf("%s\n", matches[ii]);
        res = matchPermissionDb(db, matches[ii]);
        //We see if any of the permissions are already in use
        if(res == 0) {
        	printf("Permission \"%s\" is already in use\n", matches[ii]);
        	return -1;
        }
    }
    insertPermissionDb(db, "digital_write");
    char buff[100];
	sprintf(buff,"am start -n com.example.arnauprat.test20/com.example.arnauprat.test20.MainActivity");
    system(buff);
    printf("inserting\n");
    
    //launch application

	return 0;
}

int halt(sqlite3 *db, char *filename)
{
	deletePermissionDb(db, "digital_write");
	return 0;
}

int main(int argc, char const *argv[])
{
   /*
	*
	*   init : create permissions table (if not exist) and install application (??)
	*
    *   run: check if permissions in use, put permissions database, run application
	*
    *   halt: stop application and delete permissions from the database
	*
    *   setup: in this case it does nothing
	*
    */

    int res = 0;

	sqlite3 *database;

	//Init function
	init(database, "app-release.apk");

	//Run function
	res = run(database, "app-release.apk");

	printf("Result run 1: %d\n", res);

	//Run function
	res = run(database, "app-release.apk");

	printf("Result run 2: %d\n", res);

	//Halt function

	halt(database, "app-release.apk");

	//Run function

	res = run(database, "app-release.apk");

	printf("Result run 3: %d\n", res);

    return 0;
}