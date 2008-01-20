PACKAGE=cgrok

CPP=g++
CFLAGS=-g -I/usr/local/include
#CFLAGS=-O2 -I/usr/local/include

all: grok

grok: main.o
	$(CPP) main.o -o $@

patfind: pattern_discovery.o
	$(CPP) pattern_discovery.o -o $@

clean:
	rm *.o grokre test test_patterns > /dev/null 2>&1 || true

pattern_discovery.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp
main.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp
grokpatternset.hpp: grokpattern.hpp

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

package:
	@svn status -q | wc -l | awk '$$1 > 0 { print "svn changes are not checked in!"; exit(1) }'
	make -C t/ test
	PACKAGE=$(PACKAGE) sh release.sh

