AM_CFLAGS = $(WARNINGCFLAGS)
AM_LDFLAGS = -pthread

bin_PROGRAMS = sdb

sdb_SOURCES = sdb_private.h sdb.c sdb_director.c sdb_module.c sdb_observer.c
sdb_CPPFLAGS = -I$(top_srcdir)/src/Architecture/include
sdb_LDADD = $(top_srcdir)/src/Architecture/Libraries/mcs/libmcs.la
