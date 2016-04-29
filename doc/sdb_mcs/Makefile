PDF = sdb_mcs.pdf
SOURCES = sdb_mcs.tex

AUX = $(SOURCES:%.tex=%.aux)
LOG = $(SOURCES:%.tex=%.log)

all: $(PDF)

clean: $(SUBCLEAN)
	rm -rf $(AUX) $(LOG) $(PDF)

%.pdf: %.tex
	$(LATEX_CC) -interaction=nonstopmode $<

.PHONY: all clean
