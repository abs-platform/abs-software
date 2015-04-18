APPLICATION = sdb
SOURCES = sdb.c sdb_director.c sdb_module.c sdb_observer.c
SUBDIRS = Test

LOCAL_LDFLAGS = -lmcs -pthread -labs

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)
SUBTEST = $(addsuffix .test,$(SUBDIRS))
SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(APPLICATION) $(SUBDIRS)
	for app in $(APPLICATION); do cp $$app $(BIN_DIR)/Architecture/SDB; done;

test: $(SUBTEST)

clean: $(SUBCLEAN)
	rm -rf $(OBJS) $(DEPS)

$(APPLICATION): $(OBJS)
	$(GUEST_CC) $^ -o $@ $(LIB_FLAG) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(GUEST_CC) -M -MF $*.d $< $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)
	$(GUEST_CC) -c $< -o $@ $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST) 
