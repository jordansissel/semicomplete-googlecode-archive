
LIBS=-L/usr/lib -lpcap -L/usr/local/lib -pthread
INCLUDE=-I/usr/local/include

BUILDY=gcc -g ${INCS} ${LIBS} -o proxy

SRCS=main.c log.c hash.c
OBJS=main.o log.o hash.o

CFLAGS+=-g

CFLAGS+=$(INCLUDE)

all: $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) -o proxy

clean:
	rm $(OBJS) >/dev/null 2>&1 || true;

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

