EXE_HOST = mcsg
OUT_HOST = auto_mcs.h
SOURCES_HOST = mcsg.c cJSON.c
SUBDIRS = Test

LOCAL_LDFLAGS_HOST = -lm

OBJS_HOST = $(SOURCES_HOST:%.c=%.o_h)
DEPS_HOST = $(SOURCES_HOST:%.c=%.d)
SUBTEST = $(addsuffix .test,$(SUBDIRS))
SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(OUT_HOST) $(SUBDIRS)

test: $(SUBTEST)

clean: $(SUBCLEAN)
	rm -rf $(OBJS_HOST) $(DEPS_HOST) $(OUT_HOST) $(EXE_HOST)

$(OUT_HOST): $(EXE_HOST)
	for exe in $(EXE_HOST); do ./$$exe; done;
	for out in $(OUT_HOST); do cp $$out $(BIN_DIR)/Architecture/Include; done;

$(EXE_HOST): $(OBJS_HOST)
	$(HOST_CC) $^ -o $@ $(LOCAL_LDFLAGS_HOST)

-include $(DEPS)
-include $(DEPS_HOST)

%.o_h: %.c
	$(HOST_CC) -M -MF $*.d $< $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS_HOST)
	$(HOST_CC) -c $< -o $@ $(INCLUDE_FLAG) $(CFLAGS) $(LOCAL_CFLAGS_HOST)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST) 
