
CC=gcc
SOURCES= graph.c kruskal.c main.c 
#linkedlist.c
NUMTESTS=1
CFLAGS=-g -DNUMTESTS=$(NUMTESTS)

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o) kruskal
purify:
	@echo source: /usr/local/Rational2003/*.ksh
	make NUMTESTS=1 CC="purify gcc" clean all

clean:
	rm -f $(SOURCES:%.c=%.o)
	rm -f testgraph kruskal testll

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

gtest: testgraph.o graph.o
	$(CC) $(CFLAGS) -o gtest testgraph.o graph.o

testll: 
	$(CC) $(CFLAGS) -DTEST -o $@ linkedlist.c

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

kruskal: main.o graph.o kruskal.o 
	$(CC) $(CFLAGS) main.o graph.o kruskal.o -o kruskal


