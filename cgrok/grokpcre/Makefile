#CFLAGS+=-pipe -O2
CFLAGS+=-pipe -O2
CFLAGS+=-Wunused
#LDFLAGS+=-static

GROKRE_DEPS=grokre.o stringhelper.o predicates.o

all: clean grokre

test:
	make -C test

clean:
	rm -f grokre *.o *.so

grokre:  $(GROKRE_DEPS)
	$(CC) $(LDFLAGS) $(GROKRE_DEPS) -lpcre -o grokre

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
