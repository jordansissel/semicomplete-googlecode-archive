LDFLAGS+=-lcunit

BINARIES=test_stringhelper

test:
	@echo "Nothing to do yet."

clean:
	rm -f *.o $(BINARIES)

%.c: gentest.sh

test_stringhelper.o: test_stringhelper.c ../stringhelper.c
	rm -f _$<
	./gentest.sh $< >> _$<
	$(CC) -c _$< -o $@;
	rm _$<

test_stringhelper: test_stringhelper.o ../stringhelper.o
