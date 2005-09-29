
WORKDIR=.generate
XSLTDIR=/u9/psionic/public_html/new

XMLLINT=xmllint
XSLTPROC=xsltproc

PROCESSXSL=$(WORKDIR)/.xsl_processed

website: includedir webroot subdirs

includedir: $(shell ls -d include/*.xml | sed -e 's/$$/i/')

webroot: $(shell ls -d *.xml | sed -e 's/xml$$/html/')

subdirs:
	@for i in `ls -d *`; do \
		[ -d $$i -a -f $$i/Makefile ] && $(MAKE) -C $$i ; \
	done; \
	:

clean:
	-rm *.html
	-find ./ -name '.generate' -type d | xargs -n 1 rm -rf 

$(PROCESSXSL): $(WORKDIR) $(shell ls -d *.xsl | sed -e 's,^,$(WORKDIR)/,)
	@echo "($@) Changes: $?"
	@touch "$@"

$(WORKDIR):
	@mkdir $(WORKDIR) >/dev/null 2>&1 || true

%.html: %.xml $(WORKDIR) $(PROCESSXSL)
	@echo "=== Generating $@"
	@echo "New: $?"
	@echo "Total: $^"
	$(XMLLINT) --xinclude $< > $(WORKDIR)/$<
	$(XSLTPROC) $(WORKDIR)/$< > $(WORKDIR)/$@
	[ -f "$@" ] && cp $@ $@.bak || true
	cp $(WORKDIR)/$@ ./$@
	

include/%.xmli: include/%.xml
	$(XMLLINT) --xinclude $< > $@

$(WORKDIR)/%.xsl: %.xsl 
	$(XMLLINT) --xinclude $< > $@;
