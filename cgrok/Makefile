#CFLAGS+=-pipe -O2
CFLAGS+=-pipe -O2
CFLAGS+=-Wunused
#LDFLAGS+=-static

GROKRE_DEPS=grokre.c stringhelper.c predicates.c logging.c

all: clean grokre

test:
	make -C test

main: main.o $(GROKRE_DEPS:.c=.o)
	gcc -o $@ -lpcre $< $(GROKRE_DEPS)

clean:
	rm -f grokre *.o *.so

grokre:  $(GROKRE_DEPS)
	$(CC) $(LDFLAGS) $(GROKRE_DEPS) -lpcre -o grokre

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
