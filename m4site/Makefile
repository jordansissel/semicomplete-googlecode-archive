
TARGETDIR=/u9/psionic/public_html/new
SHELL=/bin/sh

ECHO=/usr/ucb/echo

.SUFFIXES: .in .html

default:
	@make `echo *.in | sed -e 's/in$$/html/'`
	@for i in *; do \
		[ -d "$$i" -a -f "$$i/Makefile" ] && (cd $$i; make); \
	done; :

.in.html: 
	m4 generate.m4 -DINPUTPAGE="$<" > $@

test.html: header.m4 footer.m4 macros.m4
header.m4: logo.m4 menu.m4
