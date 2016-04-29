APPLICATION = sdb
SOURCES = sdb.c sdb_director.c sdb_module.c sdb_observer.c sdb_queue.c sdb_usb.c sdb_qos.c
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
	rm -rf $(OBJS) $(DEPS) $(APPLICATION)

$(APPLICATION): $(OBJS)
	$(ANDROID_CC) $^ -o $@ $(ANDROID_LDFLAGS) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(ANDROID_CC) -M -MF $*.d $< $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)
	$(ANDROID_CC) -c $< -o $@ $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST)
