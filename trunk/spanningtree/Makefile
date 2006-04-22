CC=gcc
SOURCES= graph.c

.SUFFIXES: .o .c

all: $(SOURCES:%.c=%.o)

clean:
	rm $(SOURCES:%.c=%.o)

testgraph: 
	$(CC) $(CFLAGS) -DTEST -o testgraph graph.c

.c.o:
	$(CC) $(CFLAGS) -o $@ $<
