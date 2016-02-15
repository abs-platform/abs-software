include configure.mk
APPLICATION = bcd
SOURCES = bcd.c

#LOCAL_LDFLAGS = -lmcs -pthread -labs

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)

all: $(APPLICATION)

clean: $(SUBCLEAN)
	rm -rf $(OBJS) $(DEPS) $(APPLICATION)

$(APPLICATION): $(OBJS)
	$(GUEST_CC) $^ -o $@ $(LIB_FLAG) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(GUEST_CC) -M -MF $*.d $< $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)
	$(GUEST_CC) -c $< -o $@ $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS)

.PHONY: all clean
