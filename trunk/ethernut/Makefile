# GCART Ethernut i2c framework Makefile
#
# $Id$

CC=gcc
CFLAGS+=-fno-builtin-log -Wall -O2

# For Linux and FreeBSD
#LDFLAGS+=-pthread

# For solaris
LDFLAGS+=-lpthread -lsocket -lnsl

OBJECTS=main.o busmaster.o log.o network.o i2c.o

all: ethernut

clean:
	rm $(OBJECTS) 2> /dev/null || true
	rm ethernut 2> /dev/null || true

ethernut: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o ethernut

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

