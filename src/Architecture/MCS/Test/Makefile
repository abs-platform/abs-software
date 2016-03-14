TEST = mcs
SOURCES = mcs.c

LOCAL_LDFLAGS = -labs -labs_test -lmcs

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)

all:

test: $(TEST)
	for test in $(TEST); do ./$$test; done;

clean:
	rm -rf $(OBJS) $(DEPS)

$(TEST): $(OBJS)
	$(GUEST_CC) $^ -o $@ $(LIB_FLAG) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(GUEST_CC) -M -MF $*.d $< $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)
	$(GUEST_CC) -c $< -o $@ $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)

.PHONY: all clean test
