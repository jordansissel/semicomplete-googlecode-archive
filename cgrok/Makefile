
CFLAGS+=-g -I/usr/local/include
LDFLAGS+=-L/usr/local/lib -lonig

grokre: grokre.o grokmatch.o grokregex.o
	g++ $(LDFLAGS) grokre.o grokmatch.o grokregex.o -o $@

clean:
	rm *.o grokre > /dev/null 2>&1 || true

.cc.o:
	g++ $(CFLAGS) -c -o $@ $<


