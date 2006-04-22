CC=gcc
SOURCES= graph.c

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o)

clean:
	rm -f $(SOURCES:%.c=%.o)
	rm -f testgraph

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

.c.o:
	$(CC) $(CFLAGS) -o $@ $<
