CFLAGS=

DYNFLAGS="-fPIC -shared"

socket.so: socket.o
	@CFLAGS="$(CFLAGS)"; \
	[ "`uname`" = "Linux" ] && CFLAGS="$${CFLAGS} -ldl"; \
	gcc $(DYNFLAGS) $${CFLAGS} socket.o -o socket.so


socket.o: socket.c
	@CFLAGS="$(CFLAGS); \
	gcc socket.c -o socket.o
