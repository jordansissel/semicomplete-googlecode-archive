#CFLAGS+=-pipe -O2
#CFLAGS+=-g
CFLAGS+=-Wunused
LDFLAGS+=-lpcre -ldb
CLEANOBJ=*.o *_xdr.[ch]
CLEANBIN=main grokre

GROKRE_DEPS=grokre.c stringhelper.c predicates.c logging.c \
            grok_pattern.c

all: clean grokre

main: main.o $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean 
clean: cleanobj cleanbin

.PHONY: cleanobj
cleanobj:
	rm -f $(CLEANOBJ)

.PHONY: cleanbin
cleanbin:
	rm -f $(CLEANBIN)

grokre: $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) $^ -o $@

grok_capture_xdr.o: grok_capture_xdr.c grok_capture_xdr.h
grok_capture_xdr.c: grok_capture.x
	rpcgen -c $< -o $@
grok_capture_xdr.h: grok_capture.x
	rpcgen -h $< -o $@

grok.h: grok_capture.h
grok_capture.h: grok_capture_xdr.h
grok_capture.o: grok_capture.h
grok.o: grok.h grok_capture.h

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
