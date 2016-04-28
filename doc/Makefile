SUBDIRS = firmware sdb_libs sdb_mcs

SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

all: $(SUBDIRS)

test: $(SUBTEST)

clean: $(SUBCLEAN)

$(SUBDIRS):
	$(MAKE) -C $@

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean

.PHONY: all clean test $(SUBDIRS) $(SUBCLEAN)
