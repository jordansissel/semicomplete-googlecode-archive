
CXXGEN=cxxtest/cxxtestgen.pl

CFLAGS=-I./cxxtest -I../

.SUFFIXES: .test .cpp .o

foo: foo.o
	$(CXX) $(LDFLAGS) $< -o $@

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

.test.cpp:
	$(CXXGEN) --error-printer -o $@ $<
