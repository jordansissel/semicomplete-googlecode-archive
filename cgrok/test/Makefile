include ../Makefile

# Search these paths for dependency targets
VPATH=..:.

LDFLAGS+=-lcunit
CFLAGS+=-I. -I.. -DNOLOGGING -g
CLEANOBJ=*.o *.gentest.c *_xdr.[hco]
CLEANBIN=*.test

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

grok_pattern.test: CFLAGS+=-DGROK_TEST_NO_CAPTURE
grok_pattern.test: grok.o grok_pattern.o

grok_capture.test: grok.o grok_capture.o grok_capture_xdr.o
ct.test: grok.o grok_capture.o grok_capture_xdr.o
grok_simple.test: grok.o grok_capture.o grok_pattern.o
predicates.test:
stringhelper.test:

%.test: cleanobj %.test.o 
	$(CC) $(LDFLAGS) $(CFLAGS) $(^:cleanobj=) -o $@
