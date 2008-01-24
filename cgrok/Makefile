PACKAGE=cgrok

CPP=g++
CFLAGS=-I/usr/local/include

CFLAGS+=-g
#CFLAGS+=-O2

all: grok

grok: main.o
	$(CPP) $< -o $@

pattest: pattern_test.o
	$(CPP) pattern_test.o -o $@

patfind: pattern_discovery.o
	$(CPP) pattern_discovery.o -o $@

clean:
	rm *.o grokre test test_patterns patfind grok > /dev/null 2>&1 || true

pattern_discovery.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp
main.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp grokconfig.hpp
grokpatternset.hpp: grokpattern.hpp

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

package:
	@svn status -q | wc -l | awk '$$1 > 0 { print "svn changes are not checked in!"; exit(1) }'
	make -C t/ clean test
	PACKAGE=$(PACKAGE) sh release.sh

