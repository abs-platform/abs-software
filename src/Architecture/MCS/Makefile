APPLICATION = mcsg
JSON = mcsg.json
OUT = auto_mcs.h auto_mcs.java
SOURCES = mcsg.c cJSON.c
SUBDIRS = Test

LOCAL_LDFLAGS = -lm

OBJS = $(SOURCES:%.c=%.o)
DEPS = $(SOURCES:%.c=%.d)
SUBTEST = $(addsuffix .test,$(SUBDIRS))
SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(OUT) $(SUBDIRS)

test: $(SUBTEST)

clean: $(SUBCLEAN)
	rm -rf $(OBJS) $(DEPS) $(OUT) $(APPLICATION)

$(OUT): $(APPLICATION) $(JSON)
	for exe in $(APPLICATION); do ./$$exe; done;
	for out in $(OUT); do cp $$out $(BIN_DIR)/Architecture/Include; done;

$(APPLICATION): $(OBJS)
	$(HOST_CC) $^ -o $@ $(HOST_LDFLAGS) $(LOCAL_LDFLAGS)

-include $(DEPS)

%.o: %.c
	$(HOST_CC) -M -MF $*.d $< $(HOST_CFLAGS)
	$(HOST_CC) -c $< -o $@ $(HOST_CFLAGS)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST)
