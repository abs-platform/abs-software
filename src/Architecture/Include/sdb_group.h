#ifndef __SDB_GROUP_H
#define __SDB_GROUP_H

#include <abs.h>

typedef enum SDBGroup {
    SDB_GROUP_SYSCORE,
    SDB_GROUP_PROCMAN,
    SDB_GROUP_SDB,
    SDB_GROUP_HWMOD,
    SDB_GROUP_APP,
    SDB_GROUP_MAX,
    SDB_GROUP_ANY, /* Special group. Nobody can be in this group. */
} SDBGroup;

static const unsigned int sdb_group_priority[SDB_GROUP_MAX] = {
    0,
    1,
    1,
    99,
    100,
};

enum_string(SDBGroup, sdb_group_conversion)
    {SDB_GROUP_SYSCORE, "syscore"},
    {SDB_GROUP_PROCMAN, "procman"},
    {SDB_GROUP_SDB,     "sdb"},
    {SDB_GROUP_HWMOD,   "hwmod"},
    {SDB_GROUP_APP,     "app"},
};

#endif
