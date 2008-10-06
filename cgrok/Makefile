#CFLAGS+=-pipe -O2
#CFLAGS+=-g
CFLAGS+=-Wunused
LDFLAGS+=-lpcre
CLEANARGS=*.o grokre *_xdr.c

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

grok_capture_xdr.c: grok_capture.x
	rpcgen $<

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
