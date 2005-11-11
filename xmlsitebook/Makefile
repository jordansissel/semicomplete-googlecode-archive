.SUFFIXES: .fo .xml .pdf
TARGETS=sitebook.pdf testplan.pdf

all: $(TARGETS)

clean:
	-rm $(TARGETS) testplan.fo sitebook.fo

.xml.fo: sitebook.xsl
	xsltproc $< > $@

.fo.pdf:
	fop $< $@
