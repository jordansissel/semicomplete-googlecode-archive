CC=gcc
SOURCES= graph.c kruskal.c main.c
CFLAGS=-g

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o)

clean:
	rm -f $(SOURCES:%.c=%.o)
	rm -f testgraph kruskal

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

kruskal: main.o graph.o kruskal.o
	gcc main.o graph.o kruskal.o -o kruskal
