
WORKDIR=.generate
XSLTDIR=/u9/psionic/public_html/new

XMLLINT=xmllint
XSLTPROC=xsltproc
#XSLTPROC=xsltproc --path "$(XSLTDIR)"

PROCESSXSL=$(WORKDIR)/.xsl_processed

website: webroot subdirs

webroot: $(shell ls -d *.xml | sed -e 's/xml$$/html/')
	@echo "($@) Changes: $?"

subdirs:
	@echo "=== Recursing directories"
	@for i in `ls -d *`; do \
		[ -d $$i -a -f $$i/Makefile ] && $(MAKE) -C $$i ; \
	done; \
	:
	@echo "=== DONE ==="

clean:
	rm *.html
	rm -rf $(WORKDIR)/

$(PROCESSXSL): $(WORKDIR) $(shell ls -d *.xsl | sed -e 's,^,$(WORKDIR)/,' -e 's/$$/.p/')
	@echo "($@) Changes: $?"
	@echo "($@) All: $^"
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
	
$(WORKDIR)/%.xsl.p: %.xsl 
	$(XMLLINT) --xinclude $< > $@;
