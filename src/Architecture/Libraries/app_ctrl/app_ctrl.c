#include <stdio.h>
#include <stdlib.h>
#include "app_ctrl.h"
#include "app_ctrl_assets.h"

#define dbName "permissions.db"

/* gcc app_ctrl.c app_ctrl_assets.c JUnzip/junzip.c sqlite/sqlite3.c -lz -DDEBUG=1 -o out */

int init_appctrl()
{
	int res;
    db = openDb(dbName);
    res = createPermissionsTable(db);
    //install application
	return res;
}

int app_install(char *filename)
{
	char buffer[1000];
	/* install application */
    sprintf(buffer, "am install %s", filename);
    system(buffer);
	return 0;
}

int app_uninstall(char *filename)
{
	char buffer[1000];
	/* install application */
    sprintf(buffer, "pm uninstall %s", filename);
    system(buffer);
	return 0;
}

int app_launch(char *filename)
{
	char *pakage = "com.example.arnauprat.test20/com.example.arnauprat.test20.MainActivity";
	char buffer[1000];
	int i, r, res, size;
	unsigned char *data;
	/* Get permissions from APK*/   
    size = getManifestFromAPK(filename, &data);
    char **matches = getPermissionsFromManifest(data, size, &res);

    /* Check if permissions are in use */
    for(i = 0; i < res; i++) {
        printf("%s\n", matches[i]);
        r = findPermissionDb(db, matches[i]);
        //We see if any of the permissions are already in use
        if(r == 0) {
        	printf("Permission \"%s\" is already in use\n", matches[i]);
        	return -1;
        }
    }
    /* if none of the permissions are in the daatabase add them */

    for(i = 0; i < res; i++) {
    	addPermissionDb(db, matches[i]);
        printf("Permission \"%s\" added\n", matches[i]);
        
    }
    /* launch application */
    sprintf(buffer, "am start -n %s", pakage);
    system(buffer);
	return 0;
}

int app_exit(char *filename)
{
	char *pakage = "com.example.arnauprat.test20/com.example.arnauprat.test20.MainActivity";
	char buffer[1000];
	int i, res, size;
	unsigned char *data;   
    size = getManifestFromAPK(filename, &data);
    char **matches = getPermissionsFromManifest(data, size, &res);
    /* delete the permission used by the app on the database */
    deleteApplicationPermissionsdB(db, filename);
    /* exit application */
	sprintf(buffer,"am kill -n %s", pakage);
    system(buffer);
	return 0;
}

int main(int argc, char const *argv[])
{
   /*
	*
	*	installApp: install application
	*	startApp: check permissions and if not in use start app
	*	stopApp: delete permissions and stop application
	*	uninstallApp: uninstall application
    */

    int res = 0;
	char *appName = "app-release.apk";
	//Init function
	init_appctrl();
	//Run function
	res = app_launch(appName);
	printf("Result run 1: %d\n", res);
	//Run function
	res = app_launch(appName);
	printf("Result run 2: %d\n", res);
	//Halt function
	res = app_exit(appName);
	//Run function
	res = app_launch(appName);
	printf("Result run 3: %d\n", res);
    return 0;
}