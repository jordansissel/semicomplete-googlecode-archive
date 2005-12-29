
DEPTH=1
.SUFFIXES: .xml

SUBTARGETS=articles
CLEANTARGETS+=clean-articles

include ../Makefile

articles: index.html
	@for i in `ls -d *`; do \
	if [ -d $$i ]; then \
		$(MAKE) $$i/index.html; \
	fi; \
	done;

# Generate index.in
index.in: header.xsl */index.xml
	xsltproc header.xsl */index.xml > $@

clean-articles:
	rm index.html index.in */index.html || true

.xml.in:
	xmllint $< > /dev/null
	[ -f "$@" ] && cp $@ $(@)~ || true
	xsltproc article.xsl $< > $@
	cp $< $<.ok

