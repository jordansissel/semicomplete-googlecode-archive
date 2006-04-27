CC=gcc
SOURCES= graph.c kruskal.c main.c linkedlist.c
CFLAGS=-g

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

kruskal: main.o graph.o kruskal.o linkedlist.o
	gcc main.o graph.o kruskal.o linkedlist.o -o kruskal


