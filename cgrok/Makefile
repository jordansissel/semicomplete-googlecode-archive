
CFLAGS+=-g -I/usr/local/include
LDFLAGS+=-L/usr/local/lib -lonig

grokre: grokre.o grokmatch.o grokregex.o
	g++ $(LDFLAGS) grokre.o grokmatch.o grokregex.o -o $@

.o: .cc
	g++ $(CFLAGS) $< -o $@
