
CC=gcc
SOURCES=graph.c kruskal.c main.c carp.c prim.c

MSTSOURCES=graph.c kruskal.c prim.c carp.c main.c
CFLAGS=-g
#-fast

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o) kruskal
purify:
	@echo source: /usr/local/Rational2003/*.ksh
	make CC="purify gcc" clean all

clean:
	rm -f $(SOURCES:%.c=%.o)
	rm -f testgraph kruskal testll gtest

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

gtest: testgraph.o graph.o
	$(CC) $(CFLAGS) -o gtest testgraph.o graph.o

testll: 
	$(CC) $(CFLAGS) -DTEST -o $@ linkedlist.c

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

kruskal: main.o graph.o kruskal.o carp.o
	$(CC) $(CFLAGS) main.o graph.o kruskal.o carp.o -o kruskal

mst:	$(MSTSOURCES:%.c=%.o)
	$(CC) $(CFLAGS) $(MSTSOURCES:%.c=%.o) -o mst
