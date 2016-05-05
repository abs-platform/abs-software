LIBRARIES = libsdb.so
SOURCES = sdb.c
SUBDIRS = Test

LOCAL_CFLAGS = -g -fpic
LOCAL_LDFLAGS = -lmcs -shared -pthread

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)

SUBTEST = $(addsuffix .test,$(SUBDIRS))
SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(LIBRARIES)
	for lib in $(LIBRARIES); do cp $$lib $(LIB_DIR); done;

test: $(SUBTEST)

clean: $(SUBCLEAN)
	rm -rf $(OBJS) $(DEPS) $(LIBRARIES)

$(LIBRARIES): $(OBJS)
	$(ANDROID_CC) $< -o $@ $(ANDROID_LDFLAGS) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(ANDROID_CC) -M -MF $*.d $< $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)
	$(ANDROID_CC) -c $< -o $@ $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST)
