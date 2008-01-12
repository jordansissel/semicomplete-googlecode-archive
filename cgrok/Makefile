
CPP=g++
CFLAGS=-g -I/usr/local/include

test: main.o grokregex.o patterns.o
	$(CPP) main.o grokregex.o patterns.o -o $@

test_patterns: main.o grokpatterns.o
	$(CPP) main.o grokpatterns.o -o $@

clean:
	rm *.o grokre > /dev/null 2>&1 || true

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

