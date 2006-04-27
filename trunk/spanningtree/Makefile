CC=gcc
SOURCES= graph.c kruskal.c main.c 
#linkedlist.c
NUMTESTS=1
CFLAGS=-Wall -g -DNUMTESTS=$(NUMTESTS)

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o) kruskal

clean:
	rm -f $(SOURCES:%.c=%.o)
	rm -f testgraph kruskal testll

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

testll: 
	$(CC) $(CFLAGS) -DTEST -o $@ linkedlist.c

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

kruskal: main.o graph.o kruskal.o 
	gcc main.o graph.o kruskal.o -o kruskal


