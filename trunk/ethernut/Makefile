# GCART Ethernut i2c framework Makefile
#
# $Id$

OBJECTS=main.o busmaster.o

all: ethernut

ethernut: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o ethernut

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

