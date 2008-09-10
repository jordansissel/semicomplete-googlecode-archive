
test:
	@echo "Nothing to do yet."

clean:
	rm -f *.o

test_stringhelper.o: test_stringhelper.c ../stringhelper.c
	cp $< _$<
	./gentest.sh $< >> _$<
	$(CC) -c _$< -o $@;
	rm _$<

test_stringhelper: test_stringhelper.o ../stringhelper.o
