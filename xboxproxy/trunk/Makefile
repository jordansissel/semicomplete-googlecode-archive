#LIBNET_CFLAGS=-DLIBNET_BSDISH_OS -DLIBNET_BSD_BYTE_SWAP -DHAVE_SOCKADDR_SA_LEN -DLIBNET_LIL_ENDIAN -D__GLIBC__
#LIBNET_LDFLAGS=-lnet

.if defined(INCLUDE_BUILDFLAGS)
.include "Makefile.inc"
.endif

LIBS+=-L/usr/lib -lpcap -L/usr/local/lib -pthread
INCLUDE=-I/usr/local/include

BUILDY=gcc -ansi -g ${INCS} ${LIBS} -o proxy

SRCS=main.c log.c hash.c
OBJS=main.o log.o hash.o

LIBNET_CFLAGS_HACK=-D__GLIBC__

CFLAGS+=-g -Wall $(LIBNET_CFLAGS) $(BUILDFLAGS) $(LIBNET_CFLAGS_HACK)
CFLAGS+=$(INCLUDE)

default: config

config: Makefile.inc
	@make all -DINCLUDE_BUILDFLAGS

Makefile.inc: configure
	@sh configure

all: $(OBJS) proxy

proxy:
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) /usr/local/lib/libnet.a -o proxy

clean:
	rm -f $(OBJS) proxy *.core Makefile.inc >/dev/null 2>&1 || true;

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

