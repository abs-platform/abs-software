SUBDIRS =

ifdef build_arduino
SUBDIRS += Arduino
endif

ifdef build_android_java
SUBDIRS += Android
endif

ifdef build_android_c
SUBDIRS += Architecture
endif

SUBTEST = $(addsuffix .test,$(SUBDIRS))
SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(SUBDIRS)

test: $(SUBTEST)

clean: $(SUBCLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBTEST): %.test:
	$(MAKE) -C $* test

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST)
