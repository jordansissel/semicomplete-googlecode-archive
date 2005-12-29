
TARGETDIR=/u9/psionic/public_html/new
#SHELL=/bin/ksh
SHELL=/bin/ksh

#MAKEFLAGS=--no-print-directory
#MAKEFLAGS=-r

M4=/usr/local/bin/m4
TEST=/bin/test
ECHO=/usr/ucb/echo

.SUFFIXES: .in .html .xml .xin

INDEPS= $(BACKPATH)dirlinks.m4 \
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
	@for i in *; do \
		if [ -d "$$i" -a -f "$$i/Makefile" ]; then \
			cd $$i; \
			$(MAKE) clean; \
		fi; \
	done;

# Hack?
$(BACKPATH)dirlinks.m4: $(BACKPATH)$(TEMPDIRLINKS)
	@echo GOING
	@cp $< $@

$(BACKPATH)$(TEMPDIRLINKS): FORCE
	@\
	[ ! -z "$(BACKPATH)" ] && exit 0 || :; \
	DIRS=`ls -d $(BACKPATH)*/`; \
	for i in $$DIRS; do \
		if $(TEST) $(BACKPATH)$(TEMPDIRLINKS) -ot $$i; then \
			echo "$$i is new (vs $(BACKPATH)$(TEMPDIRLINKS))"; \
			REBUILD=1; \
		fi; \
	done; \
	if [ ! -z "$$REBUILD" ]; then \
		echo "* Generating ($(TEMPDIRLINKS))dirlinks.m4"; \
		for i in $$DIRS; do \
			i=$${i%/}; \
			echo "define(\`LINKS_$$i',\`"; \
			ls -d $${i}/*/ 2> /dev/null | sed -e "s%^[^/]*/\(.*\)%PROJECTLINK($$i,\1)dnl%; s%/%%g"; \
			echo "')dnl"; \
		done > $(BACKPATH)$(TEMPDIRLINKS); \
	fi
FORCE:

%.html: %.in
	@echo $< '->' $@ "(Dep: $?)"
	@$(M4) $(BACKPATH)generate.m4 -DINPUTPAGE="$<" -DBACKPATH="$(BACKPATH)" -DDEPTH="$(DEPTH)" > $@

*.html: $(INDEPS)

%.in: %.xml
	@xmllint $< > /dev/null
	@[ -f "$@" ] && cp $@ $(@)~ || true
	@xsltproc article.xsl $< > $@
	cp $@ $(@:%.xin=%.in)

# DEPENDENCIES HERE

