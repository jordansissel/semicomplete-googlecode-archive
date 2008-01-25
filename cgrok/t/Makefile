
CXXGEN=cxxtest/cxxtestgen.pl

CFLAGS=-g -I./cxxtest -I../

.SUFFIXES: .test .cpp .o

basic: basic.o
	$(CXX) $(LDFLAGS) $< -o $@

patterns: patterns.o
	$(CXX) $(LDFLAGS) $< -o $@

patterns.test: patterns.test_template pattern_test_set.generated_test
	perl -ne 'if (/%PATTERN_TESTS%/) { print `cat pattern_test_set.generated_test`; } else { print $$_ }' < patterns.test_template > $@

pattern_test_set.generated_test: ../patterns generate_pattern_test.sh
	sh generate_pattern_test.sh > $@

test: basic patterns
	./basic
	./patterns

clean:
	rm -f *.o || true
	rm basic patterns || true

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

.test.cpp:
	$(CXXGEN) --error-printer -o $@ $<
