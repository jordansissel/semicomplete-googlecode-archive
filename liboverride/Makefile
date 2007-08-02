CFLAGS=-fPIC

DYNFLAGS=-fPIC -shared

socket.so: socket.o
	@CFLAGS="$(CFLAGS)"; \
	[ "`uname`" = "Linux" ] && CFLAGS="$${CFLAGS} -ldl"; \
	set -x; \
	gcc $(DYNFLAGS) $$CFLAGS socket.o -o socket.so


socket.o: socket.c
	@CFLAGS="$(CFLAGS)"; \
	set -x; \
	gcc -c $$CFLAGS socket.c -o socket.o

clean:
	rm -f *.o *.so
