
TARGETDIR=/u9/psionic/public_html/new
#SHELL=/bin/ksh
SHELL=/bin/ksh

#MAKEFLAGS=--no-print-directory
#MAKEFLAGS=-r

M4=/usr/local/bin/m4
TEST=/bin/test
ECHO=/usr/ucb/echo

.SUFFIXES: .in .html .xml .xin

ifeq (0,${MAKELEVEL})
TOPDIR   := $(CURDIR)
MAKE     := $(MAKE) TOPDIR=$(TOPDIR)
endif

INDEPS= $(TOPDIR)/generate.m4 \
        $(TOPDIR)/layout.m4 \
        $(TOPDIR)/logo.m4 \
        $(TOPDIR)/macros.m4 \
        $(TOPDIR)/menu.m4

TARGETS=$(shell echo *.in)

TEMPDIRLINKS=.gen.dirlinks.m4

.PHONY: clean all gendirlinks subdirs

RELPATH=$(shell echo $(CURDIR) | sed -e 's,^$(TOPDIR),,' -e 's,/[^/]*,/..,g' -e 's,^/,,' -e 's,[^/]$$,&/,')

all: $(TARGETS:%.in=%.html) $(SUBTARGETS) 
	@echo "Targets: $<"

subdirs:
	@DIRS=`echo */`; \
	[ "$$DIRS" = '*/' ] && exit 0; \
	for i in $$DIRS; do \
		echo "-- $$i"; \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i; fi\
	done 2> /dev/null; :

clean: clean-local $(CLEANTARGETS)

clean-local:
	rm *.html $(TEMPDIRLINKS) dirlinks.m4 2> /dev/null; :
	@for i in */; do \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i clean; fi\
	done 2> /dev/null; :

# Hack?
dirlinks.m4: $(TOPDIR)/dirlinks.m4
$(TOPDIR)/dirlinks.m4: $(TOPDIR)/$(TEMPDIRLINKS)
	@echo GOING
	@cp $< $@

#ls -d $${i}/*/ 2> /dev/null | sed -e  "s%^[^/]*/\(.*\)%PROJECTLINK($$i,\1)dnl%; s%/%%g"
$(TOPDIR)/$(TEMPDIRLINKS): FORCE
	@echo "Generating new links list"
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

#.in.html: $(INDEPS)
#%.html: %.in
.in.html:
	@echo '*' $< '->' $@ "(Dep: $?)"
	@$(M4) $(TOPDIR)/generate.m4 -DINPUTPAGE="$<" -DRELPATH="$(RELPATH)" > $@

#*.html: $(INDEPS)

.in.xin:
	cp $< $@
.xml.xin:
	xmllint $< > /dev/null
	[ -f "$@" ] && cp $@ $(@)~ || true
	echo "TESTING"
	xsltproc article.xsl $< > $@
#cp $@ $(@:%.xin=%.in)

# DEPENDENCIES HERE

