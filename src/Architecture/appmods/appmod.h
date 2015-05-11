#ifndef __APPMOD_H
#define __APPMOD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "JUnzip/JUnzip.h"
#include "sqlite/sqlite3.h"

int init(void);
int setup(void);
int run(void);
int halt(void);

#endif /* __APPMOD_H */