include configure.mk

SUBDIRS = src

ifdef gen_docs
SUBDIRS += doc
endif

SUBTEST = $(addsuffix .test, $(SUBDIRS))
SUBCLEAN = $(addsuffix .clean, $(SUBDIRS))

all: $(SUBDIRS)

check: test

test: all $(SUBTEST)
	$(SHELL) ./test.sh $(TEST_FILE)

clean: $(SUBCLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

$(SUBTEST): %.test:
	$(MAKE) -C $* test

.PHONY: all clean check test src $(SUBDIRS) $(SUBCLEAN) $(SUBTEST)
