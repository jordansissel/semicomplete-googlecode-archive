CFLAGS+=-pipe
#CFLAGS+=-g
CFLAGS+=-O2
LDFLAGS+= -lpcre -levent -rdynamic

# Sane includes
CFLAGS+=-I/usr/local/include
LDFLAGS+=-L/usr/local/lib

# Uncomment to totally disable logging features
#CFLAGS+=-DNOLOGGING

EXTRA_CFLAGS?=
EXTRA_LDFLAGS?=
CFLAGS+=$(EXTRA_CFLAGS)
LDFLAGS+=$(EXTRA_LDFLAGS)

# For Linux
DBLIB=db
DBINC=/usr/include
LDFLAGS+=-ldl

# For FreeBSD
#DBLIB=db-4.5
#DBINC=/usr/local/include/db45

CFLAGS+=-I$(DBINC)
LDFLAGS+=-l$(DBLIB)

### End of user-servicable configuration

CLEANGEN=filters.c grok_matchconf_macro.c
CLEANOBJ=*.o *_xdr.[ch] *.yy.c *.tab.c *.tab.h
CLEANBIN=main grokre grok conftest grok_program

GROKOBJ=grok.o grokre.o grok_capture.o grok_pattern.o stringhelper.o \
        predicates.o grok_capture_xdr.o grok_match.o logging.o \
        grok_program.o grok_input.o grok_matchconf.o libc_helper.o \
        grok_matchconf_macro.o filters.o
GROKPROGOBJ=grok_input.o grok_program.o grok_matchconf.o $(GROKOBJ)

.PHONY: all
all: grok

main: $(GROKOBJ) main.o
	$(CC) $(LDFLAGS) -o $@ $^

#grok_program: LDFLAGS+=-L/usr/local/libevent/lib
#grok_program: CFLAGS+=-I/usr/local/libevent/include
grok_program: $(GROKPROGOBJ)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean 
clean: cleanobj cleanbin cleangen

.PHONY: cleanobj
cleanobj:
	rm -f $(CLEANOBJ)

.PHONY: cleanbin
cleanbin:
	rm -f $(CLEANBIN)

.PHONY: cleangen
cleangen:
	rm -f $(CLEANGEN)

grok_capture_xdr.o: grok_capture_xdr.c grok_capture_xdr.h
grok_capture_xdr.c: grok_capture.x
	[ -f $@ ] && rm $@ || true
	rpcgen -c $< -o $@
grok_capture_xdr.h: grok_capture.x
	[ -f $@ ] && rm $@ || true
	rpcgen -h $< -o $@

%.c: %.gperf
	gperf $< > $@

grok.h: grok_capture.h
grok_match.h: grok_capture.h
grok_capture.h: grok_capture_xdr.h
main.c: grok_capture.h
grok_input.c: grok_capture.h libc_helper.h
grok.c: grok.h grok_capture.h

conf.tab.c conf.tab.h: conf.y
	bison -d $<
	#yacc -d -b "conf" $<

conf.yy.c: conf.lex conf.tab.h
	flex -o $@ $<


grok: LDFLAGS+=-levent
grok: $(GROKOBJ) conf.tab.o conf.yy.o main.o grok_config.o
	gcc $(LDFLAGS) -g $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
