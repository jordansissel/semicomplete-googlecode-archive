.SUFFIXES: .fo .xml .pdf
TARGETS=sitebook.pdf

all: $(TARGETS)

clean:
	-rm sitebook.fo sitebook.pdf

.xml.fo: sitebook.xsl
	xsltproc sitebook.xsl $< > $@

.fo.pdf:
	fop $< $@
