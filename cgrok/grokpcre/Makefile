CFLAGS+=-O2 -pipe

all: clean grokre

clean:
	rm -f grokre

grokre: grokre.c
	$(CC) $(CFLAGS) -lpcre grokre.c -o grokre
