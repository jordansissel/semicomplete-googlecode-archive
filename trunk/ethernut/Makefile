# GCART Ethernut i2c framework Makefile
#
# $Id$

CC=gcc
CFLAGS+=-fno-builtin-log -Wall -g 
#-O2

# For solaris
#LDFLAGS+=-lpthread -lsocket -lnsl

# For Linux and FreeBSD
LDFLAGS+=-pthread

OBJECTS=main.o busmaster.o log.o network.o i2c.o

all: ethernut

clean:
	rm $(OBJECTS) 2> /dev/null || true
	rm ethernut 2> /dev/null || true

ethernut: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o ethernut

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

# dependencies
busmaster.o: busmaster.h common.h
i2c.o: i2c.h common.h
log.o: log.h common.h
network.o: network.h common.h


