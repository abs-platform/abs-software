TEST = sdb
SOURCES = sdb.c

LOCAL_LDFLAGS = -labs_test -lmcs -pthread

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)

all:

test: $(TEST)
	for test in $(TEST); do ./$$test; done;

clean:
	rm -rf $(OBJS) $(DEPS) $(TEST)

$(TEST): $(OBJS)
	$(ANDROID_CC) $^ -o $@ $(ANDROID_LDFLAGS) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(ANDROID_CC) -M -MF $*.d $< $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)
	$(ANDROID_CC) -c $< -o $@ $(ANDROID_CFLAGS) $(LOCAL_CFLAGS)

.PHONY: all clean test
