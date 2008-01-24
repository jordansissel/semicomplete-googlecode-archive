
CXXGEN=cxxtest/cxxtestgen.pl

CFLAGS=-g -I./cxxtest -I../

.SUFFIXES: .test .cpp .o

basic: basic.o
	$(CXX) $(LDFLAGS) $< -o $@

patterns: patterns.o pattern_test_set.generated_test
	$(CXX) $(LDFLAGS) $< -o $@

pattern_test_set.generated_test: ../patterns
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
