WORKDIR=.generate
WEBROOT=/u9/psionic/public_html/new

XMLLINT=xmllint
XSLTPROC=xsltproc
XMLMKDEP=$(WEBROOT)/build/xmlmkdep

PROCESSXSL=$(WORKDIR)/.xsl_processed

#INCLUDES=$(shell ls -d include/*.xml | sed -e 's/$$/i/')

website: .depends subdirs curdir

clean:
	-rm *.html
	-find ./ -name '$(WORKDIR)' -type d | xargs -n 1 rm -rf
	-find ./ -name '.depends' -type f | xargs -n 1 rm -f

#include .depends

# Search files for dependencies
depends: .depends
.depends: $(shell ls -d *.xsl *.xml 2> /dev/null)
	echo "YES"
	$(XMLMKDEP) $^

ifeq "$(STYLE)" "includes"
curdir: $(shell ls -d *.xml 2>/dev/null | sed -e 's/xml$$/xmli/')
else
curdir: $(shell ls -d *.xml 2>/dev/null | sed -e 's/xml$$/html/')
endif

subdirs: 
	@for i in `ls -d *`; do \
		[ -d $$i -a -f $$i/Makefile ] && $(MAKE) -C $$i ; \
	done; \
	:

$(PROCESSXSL): $(WORKDIR) #$(shell ls -d *.xsl | sed -e 's,^,$(WORKDIR)/,)
	@echo "=== Processing XSL"
	@echo "($@) Changes: $?"
	@touch "$@"

$(WORKDIR):
	@mkdir $(WORKDIR) >/dev/null 2>&1 || true

%.html: SOURCE=$<
%.html: DEST=$@
%.html: CHANGES=$?
%.html: %.xml $(WORKDIR) 
	make process SOURCE=$(SOURCE) DEST=$(DEST) CHANGES=$(CHANGES)

%.xmli: SOURCE=$<
%.xmli: DEST=$@
%.xmli: CHANGES=$?
%.xmli: %.xml $(WORKDIR)
	make process SOURCE=$(SOURCE) DEST=$(DEST) CHANGES=$(CHANGES)
 
process: $(WORKDIR)
	@echo "=== Generating $(DEST)"
	@echo "($(DEST)) Changed: $(CHANGES)"
	$(XMLLINT) --xinclude $(SOURCE) > $(WORKDIR)/$(SOURCE)
	grep '<\?xml-stylesheet' $(WORKDIR)/$(SOURCE) > /dev/null \
		&& $(XSLTPROC) $(WORKDIR)/$(SOURCE) > $(WORKDIR)/$(DEST) \
		|| cp $(WORKDIR)/$(SOURCE) $(WORKDIR)/$(DEST)
	[ -f "$(DEST)" ] && cp $(DEST) $(DEST).bak || true
	cp $(WORKDIR)/$(DEST) ./$(DEST)

$(WORKDIR)/%.xsl: %.xsl  $(WORKDIR)
	$(XMLLINT) --xinclude $< > $@;
