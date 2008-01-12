
CPP=g++
CFLAGS=-g -I/usr/local/include

test: main.o grokregex.o patterns.o
	$(CPP) main.o grokregex.o patterns.o -o $@

main.cpp: grokpatternset.hpp
grokpatternset.hpp: grokpattern.hpp

test_patterns: main.o
	$(CPP) main.o -o $@

clean:
	rm *.o grokre test test_patterns > /dev/null 2>&1 || true

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

