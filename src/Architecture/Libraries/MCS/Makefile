LIBRARIES = libmcs.so
SOURCES = mcs.c

LOCAL_CFLAGS = -g -fpic
LOCAL_LDFLAGS = -labs -shared

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)

all: $(LIBRARIES)
	for lib in $(LIBRARIES); do cp $$lib $(LIB_DIR); done;

$(LIBRARIES): $(OBJS)
	$(ANDROID_CC) $< -o $@ $(ANDROID_LDFLAGS) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(ANDROID_CC) -M -MF $*.d $< $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)
	$(ANDROID_CC) -c $< -o $@ $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)

test:

clean:
	rm -rf $(OBJS) $(DEPS) $(LIBRARIES)

.PHONY: all clean test
