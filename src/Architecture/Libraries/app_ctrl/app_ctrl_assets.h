#ifndef __ASSETS_APPMOD_H
#define __ASSETS_APPMOD_H

#include "sqlite/sqlite3.h"
#include "JUnzip/junzip.h"

#define MAX_XML_SIZE 1000

int getManifestFromAPK(char *apkname, unsigned char **manifest);

char *decompressXML(const char *xml, int cb);

char **getPermissionsFromManifest(unsigned char *data, int size, int *results);

sqlite3 *openDb(char *dbName);

int closeDb(sqlite3 *db);

int createPermissionsTable(sqlite3 *db);

int addPermissionDb(sqlite3 *db, char *permissionName);

int deletePermissionDb(sqlite3 *db, char *permissionName);

int findPermissionDb(sqlite3 *db, char *permissionName);

int deleteApplicationPermissionsdB(sqlite3 *db, char *appName);

int findApplication(sqlite3 *db, char *appName);

#endif