
TARGETDIR=/u9/psionic/public_html/new
SHELL=/bin/sh

ECHO=/usr/ucb/echo

.SUFFIXES: .in .html

all:
	@TARGETS=`echo *.in | sed -e 's/in$$/html/'`; \
	[ "$${TARGETS}" != '*.html' ] && make $$TARGETS || true
	@ \
	[ -z "$(DEPTH)" ] && DEPTH=1 || DEPTH=`expr $(DEPTH) + 1`; \
	for i in *; do \
		[ -d "$$i" -a -f "$$i/Makefile" ] && (cd $$i; DEPTH=$${DEPTH} make all); \
	done; :

clean:
	rm *.html
	@for i in *; do \
		[ -d "$$i" -a -f "$$i/Makefile" ] && (cd $$i; make clean); \
	done; :

.in.html: 
	@echo $< '->' $@
	m4 generate.m4 -DINPUTPAGE="$<" > $@

test.html: layout.m4 macros.m4
