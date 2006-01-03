
TARGETDIR=/u9/psionic/public_html/new
SHELL=/bin/ksh

M4=/usr/local/bin/m4
ECHO=/usr/ucb/echo

ifeq (0,${MAKELEVEL})
TOPMAKE   = $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
TOPDIR    = $(shell realpath `echo $(TOPMAKE) | sed -e 's/Makefile$$//' -e 's/^$$/./'`)
MAKE      := $(MAKE) TOPDIR=$(TOPDIR)
endif

INDEPS= $(TOPDIR)/generate.m4 \
        $(TOPDIR)/layout.m4 \
        $(TOPDIR)/logo.m4 \
        $(TOPDIR)/macros.m4 \
        $(TOPDIR)/menu.m4 \
	$(TOPDIR)/dirlinks.m4

TARGETS=$(shell echo *.in | sed -e 's/*.in//')

TEMPDIRLINKS=.gen.dirlinks.m4

.PHONY: clean all gendirlinks subdirs

RELPATH=$(shell echo $(CURDIR) | sed -e 's,^$(TOPDIR),,' -e 's,/[^/]*,/..,g' -e 's,^/,,' -e 's,[^/]$$,&/,')

all: $(TARGETS:%.in=%.html) $(SUBTARGETS) subdirs

subdirs:
	@DIRS=`echo */`; \
	[ "$$DIRS" = '*/' ] && exit 0; \
	for i in $$DIRS; do \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i; fi\
	done; :

clean: clean-local $(CLEANTARGETS)

clean-local:
	rm *.html $(TEMPDIRLINKS) dirlinks.m4 2> /dev/null; :
	@for i in */; do \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i clean; fi\
	done 2> /dev/null; :

$(TOPDIR)/dirlinks.m4: $(TOPDIR)/$(TEMPDIRLINKS)
	@cp $< $@

#ls -d $${i}/*/ 2> /dev/null | sed -e  "s%^[^/]*/\(.*\)%PROJECTLINK($$i,\1)dnl%; s%/%%g"
$(TOPDIR)/$(TEMPDIRLINKS): FORCE
	@\
	DIRS=`ls -d $(TOPDIR)/*/`; \
	for i in $$DIRS; do \
		i=$${i%/}; \
		echo "define(\`LINKS_$${i##*/}',\`"; \
		ls -d $$i/*/ 2> /dev/null | sed -e 's,$(TOPDIR)/,,' -e 's,/$$,,' -e 's!^\([^/]*/\(.*\)\)!LINK(\1,\2)!'; \
		echo "')dnl"; \
	done > $(TOPDIR)/$(TEMPDIRLINKS); \
	diff $(TOPDIR)/$(TEMPDIRLINKS) $(TOPDIR)/dirlinks.m4 > /dev/null 2>&1 && \
		touch -r $(TOPDIR)/dirlinks.m4 $(TOPDIR)/$(TEMPDIRLINKS) || true
FORCE:

%.html: %.in $(INDEPS)
	@echo '* $< -> $@'
	@$(M4) $(TOPDIR)/generate.m4 -DINPUTPAGE="$<" -DRELPATH="$(RELPATH)" > $@

%.in: %.xml
	@echo "* $< -> $@"
	@xmllint $< > /dev/null
	@[ -f "$@" ] && cp $@ $(@)~ || true
	@xsltproc ../article.xsl $< > $@

# Shortcuts
dirlinks.m4: $(TOPDIR)/dirlinks.m4
