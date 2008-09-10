LDFLAGS+=-lcunit -lpcre
CFLAGS+=-g -I../

BINARIES=test_stringhelper test_grok_simple

test: $(BINARIES)
	@for t in $(BINARIES); do \
		echo; \
		echo $$t; \
		./$$t | sed -e "1,6d"; \
	done

clean:
	rm -f *.o $(BINARIES)

%.c: gentest.sh

test_grok_simple.o: test_grok_simple.c ../grokre.c
	rm -f _$<
	./gentest.sh $< >> _$<
	$(CC) $(CFLAGS) -c _$< -o $@;
	rm _$<

test_stringhelper.o: test_stringhelper.c ../stringhelper.c
	rm -f _$<
	./gentest.sh $< >> _$<
	$(CC) $(CFLAGS) -c _$< -o $@;
	rm _$<

test_stringhelper: test_stringhelper.o ../stringhelper.o
test_grok_simple: test_grok_simple.o ../grokre.o ../stringhelper.o ../predicates.o
