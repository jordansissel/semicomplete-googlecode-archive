
CPP=ccache g++
CFLAGS=-I/usr/local/include -pipe
LDFLAGS+=-L/usr/local/lib -pipe

grokre: grokre.o grokmatch.o grokregex.o
	$(CPP) $(LDFLAGS) -lonig grokre.o grokmatch.o grokregex.o -o $@

boost: boost_xpressive_test.o
	$(CPP) $(LDFLAGS) boost_xpressive_test.o -o $@

clean:
	rm *.o grokre > /dev/null 2>&1 || true

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

