# GCART Ethernut i2c framework Makefile
#
# $Id$

OBJECTS=main.o busmaster.o log.o network.o

CFLAGS+=-fno-builtin-log

all: ethernut

clean:
	rm $(OBJECTS) 2> /dev/null || true
	rm ethernut 2> /dev/null || true

ethernut: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o ethernut

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

