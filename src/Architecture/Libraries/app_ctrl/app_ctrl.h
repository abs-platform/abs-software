#ifndef __APP_CTRL_H
#define __APP_CTRL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JUnzip/JUnzip.h"
#include "sqlite/sqlite3.h"



sqlite3 *db;

int init_appctrl();
int app_install(char *filename);
int app_uninstalL(char *filename);
int app_launch(char *filename);
int app_exit(char *filename);

#endif /* __APP_CTRL_H */