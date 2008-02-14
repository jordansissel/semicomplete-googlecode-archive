PACKAGE=cgrok

CPP=g++
CFLAGS=-pipe -I/usr/local/include

CFLAGS+=-g
#CFLAGS+=-O2

LDFLAGS=
#LDFLAGS=-L/usr/local/lib -lpopt

all: grok

grok: main.o
	$(CPP) $(LDFLAGS) main.o -o $@

pattest: pattern_test.o
	$(CPP) pattern_test.o -o $@

patfind: pattern_discovery.o
	$(CPP) pattern_discovery.o -o $@

filetest: filetest.o
	$(CPP) filetest.o -o $@

clean:
	rm *.o grokre test test_patterns patfind grok filetest > /dev/null 2>&1 || true

filetest.o: filetest.cpp fileobserver.hpp
pattern_discovery.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp
main.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp grokconfig.hpp fileobserver.hpp
grokpatternset.hpp: grokpattern.hpp

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

package: build-package test-package

build-package:
	#@svn status -q | wc -l | awk '$$1 > 0 { print "svn changes are not checked in!"; exit(1) }'
	PACKAGE=$(PACKAGE) sh release.sh

test-package: build-package
	PKGVER=$(PACKAGE)-`date "+%Y%m%d"`; \
	tar -C /tmp -xf $${PKGVER}.tar.gz; \
	make -C /tmp/$${PKGVER}/t test
