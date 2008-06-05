CFLAGS+=-pipe -g

all: clean grokre

clean:
	rm -f grokre

grokre: grokre.c
	$(CC) $(CFLAGS) -lpcre grokre.c -o grokre
