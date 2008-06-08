CFLAGS+=-pipe -g
CFLAGS+=-Wunused

all: clean grokre

clean:
	rm -f grokre *.o *.so

grokre: grokre.o stringhelper.o
	$(CC) -lpcre grokre.o stringhelper.o -o grokre

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
