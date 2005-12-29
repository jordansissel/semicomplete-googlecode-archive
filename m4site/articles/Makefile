
DEPTH=1
.SUFFIXES: .xml

SUBTARGETS=articles
CLEANTARGETS+=clean-articles

include ../Makefile

articles:
	@for i in `ls -d *`; do [ -d $$i ] && $(MAKE) $$i/index.html; done

clean-articles:
	@for i in `ls -d *`; do [ -d $$i ] && rm $$i/index.html; done
.xml.in:
	xmllint $< > /dev/null
	[ -f "$@" ] && cp $@ $(@)~ || true
	xsltproc article.xsl $< > $@
	cp $< $<.ok

