CFLAGS+=-pipe
#CFLAGS+=-g
#CFLAGS+=-DNOLOGGING
CFLAGS+=-O2
#CFLAGS+=-Wunused
EXTRA_CFLAGS=
CFLAGS+=$(EXTRA_CFLAGS)
LDFLAGS+=-lpcre -rdynamic
#LDFLAGS+=-L/usr/local/pcre/lib/

# For Linux
DBLIB=db
DBINC=/usr/include
LDFLAGS+=-ldl

# For FreeBSD
#DBLIB=db-4.5
#DBINC=/usr/local/include/db45
#LDFLAGS+=-L/usr/local/lib
#CFLAGS+=-I/usr/local/include

CFLAGS+=-I$(DBINC)
LDFLAGS+=-l$(DBLIB)

CLEANOBJ=*.o *_xdr.[ch] *.yy.c *.tab.c *.tab.h
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
bmain.c: grok_capture.h
grok_input.c: grok_capture.h
grok.c: grok.h grok_capture.h

grok_config.tab.c grok_config.tab.h: grok_config.grammar.y
	bison -v -d $<

grok_config.yy.c: grok_config.grammar.lex grok_config.tab.h
	flex -o $@ $<

conftest:  grok_config.tab.o grok_config.yy.o bmain.o grok_config.o
	gcc -g $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
