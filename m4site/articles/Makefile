SUBTARGETS=articles
CLEANTARGETS+=clean-articles

.PHONY: clean-articles articles

include ../Makefile

articles: index.html

# Generate index.in
index.in: header.xsl */index.xml
	@echo "<all>/index.html -> $@"
	@xsltproc header.xsl */index.xml > $@

clean-articles:
	rm index.html index.in */index.html || true

