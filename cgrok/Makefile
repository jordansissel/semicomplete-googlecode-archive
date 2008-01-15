
CPP=g++
CFLAGS=-I/usr/local/include
#CFLAGS=-O2 -I/usr/local/include

all: test_patterns

test: main.o grokregex.o patterns.o 
	$(CPP) main.o grokregex.o patterns.o -o $@

test_patterns: main.o
	$(CPP) main.o -o $@

clean:
	rm *.o grokre test test_patterns > /dev/null 2>&1 || true

main.o: grokpatternset.hpp grokregex.hpp grokmatch.hpp grokpredicate.hpp
grokpatternset.hpp: grokpattern.hpp

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

