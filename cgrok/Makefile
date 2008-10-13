CFLAGS+=-pipe
CFLAGS+=-g
#CFLAGS+=-DNOLOGGING
#CFLAGS+=-O3
#CFLAGS+=-Wunused
EXTRA_CFLAGS=
CFLAGS+=$(EXTRA_CFLAGS)
LDFLAGS+=-lpcre -ldb -ldl -rdynamic
#LDFLAGS+=-L/usr/local/pcre/lib/

CLEANOBJ=*.o *_xdr.[ch]
CLEANBIN=main grokre

GROKOBJ=grok.o grokre.o grok_capture.o grok_pattern.o stringhelper.o \
        predicates.o grok_capture_xdr.o grok_match.o logging.o
GROKPROGOBJ=grok_input.o grok_program.o grok_matchconf.o $(GROKOBJ)

all: main

main: $(GROKOBJ) main.o
	$(CC) $(LDFLAGS) -o $@ $^

grok_program: LDFLAGS+=-levent
#grok_program: LDFLAGS+=-L/usr/local/libevent/lib
#grok_program: CFLAGS+=-I/usr/local/libevent/include
grok_program: $(GROKPROGOBJ)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean 
clean: cleanobj cleanbin

.PHONY: cleanobj
cleanobj:
	rm -f $(CLEANOBJ)

.PHONY: cleanbin
cleanbin:
	rm -f $(CLEANBIN)

grok_capture_xdr.o: grok_capture_xdr.c grok_capture_xdr.h
grok_capture_xdr.c: grok_capture.x
	[ -f $@ ] && rm $@ || true
	rpcgen -c $< -o $@
grok_capture_xdr.h: grok_capture.x
	[ -f $@ ] && rm $@ || true
	rpcgen -h $< -o $@

grok.h: grok_capture.h
grok_match.h: grok_capture.h
grok_capture.h: grok_capture_xdr.h
grok_input.o: grok_capture.h
grok.o: grok.h grok_capture.h

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
