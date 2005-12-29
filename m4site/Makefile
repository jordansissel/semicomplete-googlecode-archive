
TARGETDIR=/u9/psionic/public_html/new
SHELL=/bin/sh

M4=/usr/local/bin/m4
ECHO=/usr/ucb/echo

.SUFFIXES: .in .html

default: all $(SUBTARGETS)

all:
	@TARGETS=`echo *.in | sed -e 's/\.in/.html/g'`; \
	[ "$${TARGETS}" != '*.html' ] && make $$TARGETS || :
	@ \
	[ -z "$(DEPTH)" ] && DEPTH=1 || DEPTH=`expr $(DEPTH) + 1`; \
	for i in *; do \
		if [ -d "$$i" -a -f "$$i/Makefile" ]; then  \
			(cd $$i; make DEPTH=$${DEPTH} BACKPATH=../$(BACKPATH)) \
		fi; \
	done;

clean: clean-local $(CLEANTARGETS)

clean-local:
	rm *.html || true
	@for i in *; do \
		if [ -d "$$i" -a -f "$$i/Makefile" ]; then \
			cd $$i; \
			make clean; \
		fi; \
		done;

# Hack?
#dirlinks.m4: generate_dirlinks

dirlinks.m4 gen: $(BACKPATH)articles/ $(BACKPATH)projects/
	echo "YES"
	touch dirlinks.m4
.in.html:
	@echo $< '->' $@
	$(M4) $(BACKPATH)generate.m4 -DINPUTPAGE="$<" -DBACKPATH="$(BACKPATH)" -DDEPTH="$(DEPTH)" > $@

# DEPENDENCIES HERE

test.html: $(BACKPATH)dirlinks.m4 $(BACKPATH)generate.m4


