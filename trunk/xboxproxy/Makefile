LIBNET_CFLAGS=-DLIBNET_BSDISH_OS -DLIBNET_BSD_BYTE_SWAP -DHAVE_SOCKADDR_SA_LEN -DLIBNET_LIL_ENDIAN -D__GLIBC__
LIBNET_LDFLAGS=-lnet

LIBS=-L/usr/lib -lpcap -L/usr/local/lib -pthread -lnet
INCLUDE=-I/usr/local/include

BUILDY=gcc -ansi -g ${INCS} ${LIBS} -o proxy

SRCS=main.c log.c hash.c
OBJS=main.o log.o hash.o

CFLAGS+=-g -Wall $(LIBNET_CFLAGS)

CFLAGS+=$(INCLUDE)

default: all

all: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) /usr/local/lib/libnet.a -o proxy

clean:
	rm -f $(OBJS) proxy *.core >/dev/null 2>&1 || true;

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

