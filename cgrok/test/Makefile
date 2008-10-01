LDFLAGS+=-lcunit -lpcre
CFLAGS+=-g -I../

TESTS=$(wildcard *.test.c)

test: $(TESTS:.c=)
	@for t in $(TESTS); do \
		echo; \
		echo $$t; \
		./$$t | sed -e "1,6d"; \
	done

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.gentest.c

%.c: gentest.sh


%.test.o: %.test.c 
	./gentest.sh $< > _$<
	$(CC) $(CFLAGS) -c _$< -o $@;
	rm _$<

%.test: %.test.o ../grokre.c ../stringhelper.c ../predicates.c
	$(CC) $(LDFLAGS) $(CFLAGS) $^ -o $@
