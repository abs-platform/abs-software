#ifndef __APPMOD_H
#define __APPMOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JUnzip/JUnzip.h"
#include "sqlite/sqlite3.h"

int init(sqlite3 *db, char *filename);
int setup(sqlite3 *db, char *filename);
int run(sqlite3 *db, char *filename);
int halt(sqlite3 *db, char *filename);

#endif /* __APPMOD_H */