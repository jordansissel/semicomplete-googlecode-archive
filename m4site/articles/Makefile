
DEPTH=1
.SUFFIXES: .xml

SUBTARGETS=articles
CLEANTARGETS+=clean-articles

include ../Makefile

articles:
	@for i in `ls -d *`; do [ -d $$i ] && $(MAKE) $$i/index.html || exit 1; done; :

clean-articles:
	rm */index.html

.xml.in:
	xmllint $< > /dev/null
	[ -f "$@" ] && cp $@ $(@)~ || true
	xsltproc article.xsl $< > $@
	cp $< $<.ok

