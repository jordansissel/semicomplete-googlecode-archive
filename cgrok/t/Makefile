
CXXGEN=cxxtest/cxxtestgen.pl

CFLAGS=-g -I./cxxtest -I../

.SUFFIXES: .test .cpp .o

basic: basic.o
	$(CXX) $(LDFLAGS) $< -o $@

patterns: patterns.o
	$(CXX) $(LDFLAGS) $< -o $@

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
