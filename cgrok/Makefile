#CFLAGS+=-pipe -O2
#CFLAGS+=-g
CFLAGS+=-Wunused
LDFLAGS+=-lpcre

GROKRE_DEPS=grokre.c stringhelper.c predicates.c logging.c \
            grok_pattern.c

all: clean grokre

test:
	make -C test

main: main.o $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f main grokre *.o *.so

grokre: $(GROKRE_DEPS:.c=.o)
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
