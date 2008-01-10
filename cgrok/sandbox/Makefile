
CPP=g++
CFLAGS=-I/usr/local/include

grokre: grokre.o grokmatch.o grokregex.o
	$(CPP) -L/usr/local/lib -lonig grokre.o grokmatch.o grokregex.o -o $@

xpressivetest: boost_xpressive_test.o
	$(CPP) $(LDFLAGS) boost_xpressive_test.o -o $@

clean:
	rm *.o grokre > /dev/null 2>&1 || true

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

.cc.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

