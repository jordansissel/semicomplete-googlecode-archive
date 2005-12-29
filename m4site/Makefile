
TARGETDIR=/u9/psionic/public_html/new
SHELL=/bin/sh

M4=/usr/local/bin/m4
ECHO=/usr/ucb/echo

.SUFFIXES: .in .html

	if [ "$${TARGETS}" != '*.html' ]; then \
		echo "T: $$TARGETS"; \
		make $$TARGETS; \
	fi
all:
	@TARGETS=`echo *.in | sed -e 's/\.in/.html/g'`; \
	[ "$${TARGETS}" != '*.html' ] && make $$TARGETS || :
	@ \
	[ -z "$(DEPTH)" ] && DEPTH=1 || DEPTH=`expr $(DEPTH) + 1`; \
	for i in *; do \
		[ -d "$$i" -a -f "$$i/Makefile" ] && \
		(cd $$i; make all DEPTH=$${DEPTH} BACKPATH=../$(BACKPATH)); \
	done; :

clean:
	rm *.html
	@for i in *; do \
		[ -d "$$i" -a -f "$$i/Makefile" ] && (cd $$i; make clean); \
	done; :

.in.html: 
	@echo $< '->' $@
	$(M4) $(BACKPATH)generate.m4 -DINPUTPAGE="$<" -DBACKPATH="$(BACKPATH)" -DDEPTH="$(DEPTH)" > $@

test.html: layout.m4 macros.m4
