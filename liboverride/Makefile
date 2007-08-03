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

%.so: %.o
	@CFLAGS="$(CFLAGS)"; \
	[ "`uname`" = "Linux" ] && CFLAGS="$${CFLAGS} -ldl"; \
	set -x; \
	gcc $(DYNFLAGS) $$CFLAGS $< -o $@

%.o: %.c
	@CFLAGS="$(CFLAGS)"; \
	set -x; \
	gcc -c $$CFLAGS $< -o $@

%.c-pre_sed: %.over
	m4 funcdefs $< > $@

%.c: %.c-pre_sed
	sed -E -e '/\/\/RETURNVARS/ { s,//RETURNVARS[^(]+,,; s/[[:alpha:]_]+ //g; s/[/()*]//g; }' $< \
	> $@

clean:
	rm -f *.o *.so *.c
