
DEPTH=1
.SUFFIXES: .xml

SUBTARGETS=articles

include ../Makefile

articles:
	for i in `ls -d *`; do [ -d $$i ] && $(MAKE) $$i/index.html; done

.xml.in:
	xmllint $< > /dev/null
	[ -f "$@" ] && cp $@ $(@)~ || true
	xsltproc article.xsl $< >> $@

