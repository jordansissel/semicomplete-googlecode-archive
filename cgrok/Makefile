#CFLAGS+=-pipe -O2
#CFLAGS+=-g
CFLAGS+=-Wunused
LDFLAGS+=-lpcre -ldb
CLEANARGS=*.o grokre main *_xdr.[ch]

GROKRE_DEPS=grokre.c stringhelper.c predicates.c logging.c \
            grok_pattern.c

all: clean grokre

main: main.o $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean: 
	rm -f $(CLEANARGS)

grokre: $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) $^ -o $@

grok_capture_xdr.o: grok_capture_xdr.c grok_capture_xdr.h
grok_capture_xdr.c: grok_capture.x
	rpcgen -c $< -o $@
grok_capture_xdr.h: grok_capture.x
	rpcgen -h $< -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
