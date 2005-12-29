
TARGETDIR=/u9/psionic/public_html/new
#SHELL=/bin/ksh
SHELL=/bin/ksh

#MAKEFLAGS=--no-print-directory
#MAKEFLAGS=-r

M4=/usr/local/bin/m4
TEST=/bin/test
ECHO=/usr/ucb/echo

.SUFFIXES: .in .html .xml .xin
#$(BACKPATH)dirlinks.m4 
INDEPS= \
        $(BACKPATH)generate.m4 \
        $(BACKPATH)layout.m4 \
        $(BACKPATH)logo.m4 \
        $(BACKPATH)macros.m4 \
        $(BACKPATH)menu.m4

TARGETS=$(shell echo *.in)
DEPTH?=0

TEMPDIRLINKS=.gen.dirlinks.m4

MARGS=DEPTH=$$(($(DEPTH)+1)) BACKPATH=../$(BACKPATH)

.PHONY: clean all gendirlinks subdirs

all: $(BACKPATH)dirlinks.m4 $(TARGETS:%.in=%.html) $(SUBTARGETS) subdirs

subdirs:
	@for i in */; do \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i $(MARGS); fi\
	done 2> /dev/null; :

clean: clean-local $(CLEANTARGETS)

clean-local:
	rm *.html $(TEMPDIRLINKS) dirlinks.m4 2> /dev/null; :
	@for i in */; do \
		if [ -f "$$i/Makefile" ]; then $(MAKE) -C $$i clean $(MARGS); fi\
	done 2> /dev/null; :

# Hack?
$(BACKPATH)dirlinks.m4: $(BACKPATH)$(TEMPDIRLINKS)
	@echo GOING
	@cp $< $@

$(BACKPATH)$(TEMPDIRLINKS): FORCE
	@\
	[ ! -z "$(BACKPATH)" ] && exit 0 || :; \
	DIRS=`ls -d $(BACKPATH)*/`; \
	for i in $$DIRS; do \
		i=$${i%/}; \
		echo "define(\`LINKS_$$i',\`"; \
		ls -d $${i}/*/ 2> /dev/null | sed -e "s%^[^/]*/\(.*\)%PROJECTLINK($$i,\1)dnl%; s%/%%g"; \
		echo "')dnl"; \
	done > $(BACKPATH).tmp; \
	diff $(BACKPATH).tmp $(BACKPATH)$(TEMPDIRLINKS) > /dev/null 2>&1 || \
		cp $(BACKPATH).tmp $(BACKPATH)$(TEMPDIRLINKS); \
	rm $(BACKPATH).tmp
FORCE:

#.in.html: $(INDEPS)
#%.html: %.in
.in.html:
	@echo '*' $< '->' $@ "(Dep: $?)"
	@$(M4) $(BACKPATH)generate.m4 -DINPUTPAGE="$<" -DBACKPATH="$(BACKPATH)" -DDEPTH="$(DEPTH)" > $@

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

