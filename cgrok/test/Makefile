include ../Makefile

# Search these paths for dependency targets
VPATH=../

LDFLAGS+=-lcunit
CFLAGS+=-I../ -DNOLOGGING -g
CLEANARGS=*.o *.gentest.c *.test ../*.o

TESTS=$(wildcard *.test.c)

test: $(TESTS:.c=)
	@for t in $^; do \
		echo; \
		echo $$t; \
		./$$t | sed -e "1,6d"; \
	done

%.c: gentest.sh

%.test.o: %.test.c 
	./gentest.sh $< > _$<
	$(CC) $(CFLAGS) -c _$< -o $@;
	rm _$<

grok_pattern.test: grok_pattern.o grok.o
grok_capture.test: grok_capture.o grok_capture_xdr.o grok.o
grok_simple.test:
predicates.test:
stringhelper.test:

%.test: %.test.o 
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@
