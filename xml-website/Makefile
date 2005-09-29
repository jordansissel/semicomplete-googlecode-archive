
.SUFFIXES:	.html .xml
WORKDIR=.generate
XSLTDIR=/u9/psionic/public_html/new

XMLLINT=xmllint
XSLTPROC=xsltproc
#XSLTPROC=xsltproc --path "$(XSLTDIR)"

website:
	for i in `ls -d *`; do \
		[ -d $$i -a -f $$i/Makefile ] && $(MAKE) -C $$i ; \
	done; \
	:

clean:
	rm *.html
	rm -rf .generate/

process-xsl: .generate
	for i in `ls -d *.xsl`; do \
		$(XMLLINT) --xinclude $$i > $(WORKDIR)/$$i; \
	done

.generate: 
	-mkdir .generate

.xml.html: .generate process-xsl
	@echo "=== Generating $@"
	$(XMLLINT) --xinclude $< > $(WORKDIR)/$<
	$(XSLTPROC) $(WORKDIR)/$< > $(WORKDIR)/$@
	[ -f "$@" ] && cp $@ $@.bak || true
	cp $(WORKDIR)/$@ ./$@
	

