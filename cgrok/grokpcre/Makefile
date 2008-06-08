CFLAGS+=-pipe -g

all: clean grokre

clean:
	rm -f grokre

grokre: grokre.o stringhelper.o
	$(CC) -lpcre grokre.o stringhelper.o -o grokre

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
