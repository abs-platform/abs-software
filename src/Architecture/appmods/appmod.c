#include "appmod.h"
#include "assets.h"

int init(void)
{
	return 0;
}

int setup(void)
{
	return 0;
}

int run(void)
{
	return 0;
}

int halt(void)
{
	return 0;
}

int main(int argc, char const *argv[])
{
    unsigned char *data;
    int size = getManifestFromAPK("app-release.apk", &data);
    int res;
    char **matches = getPermissionsFromManifest(data, size, &res);
    int i;
    for(i = 0; i < res; i++){
        printf("%s\n", matches[i]);
    }   

    sqlite3 *db;

    db = openDb("permissions.db");

    res = createTable(db);

    res = insertPermissionDb(db, "i2c_bus");

    res = matchPermissionDb(db, "i2c_bus");

    res = deletePermissionDb(db, "i2c_bus");

    res = matchPermissionDb(db, "i2c_bus");
    
    sqlite3_close(db);
    return 0;

    /*

        init : createTable and install application

        run: check if permissions in use, put permissions database, run application

        halt: stop application and delete permissions from the database

        setup: in this case it does nothing

    */
}