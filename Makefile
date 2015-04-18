include configure.mk

SUBDIRS = src

SUBTEST = $(addsuffix .test, $(SUBDIRS))
SUBCLEAN = $(addsuffix .clean, $(SUBDIRS))

all: $(SUBDIRS)

check: test

test: all
	$(MAKE) $(SUBTEST); STATUS=$?; $(SHELL) ./test.sh $(TEST_FILE); exit $(STATUS)

clean: $(SUBCLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

$(SUBTEST): %.test:
	$(MAKE) -C $* test

.PHONY: all clean check test $(SUBDIRS) $(SUBCLEAN) $(SUBTEST) 
