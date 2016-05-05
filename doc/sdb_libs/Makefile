PDF = sdb_libs.pdf
SOURCES = sdb_libs.tex

AUX = $(SOURCES:%.tex=%.aux)
LOG = $(SOURCES:%.tex=%.log)

all: $(PDF)

clean: $(SUBCLEAN)
	rm -rf $(AUX) $(LOG) $(PDF)

%.pdf: %.tex
	$(LATEX_CC) -interaction=nonstopmode $<

.PHONY: all clean
