#CFLAGS+=-pipe -O2
CFLAGS+=-pipe -g
CFLAGS+=-Wunused

GROKRE_DEPS=grokre.o stringhelper.o predicates.o

all: clean grokre

clean:
	rm -f grokre *.o *.so

grokre:  $(GROKRE_DEPS)
	$(CC) -lpcre $(GROKRE_DEPS) -o grokre

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
