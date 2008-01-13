
xpressive: xpressive.cpp
	g++ $< -o $@

xpressive2: xpressive.cpp
	g++ -O2 $< -o $@

xpressive3: xpressive.cpp
	g++ -O3 $< -o $@

xpressive_testgroup: xpressive_testgroup.cpp
	g++ $< -o $@

xpressive2_testgroup: xpressive_testgroup.cpp
	g++ -O2 $< -o $@

xpressive3_testgroup: xpressive_testgroup.cpp
	g++ -O3 $< -o $@

test: xpressive2 xpressive2_testgroup
	time ./xpressive2
	time ./xpressive2_testgroup
	time perl matchip.pl
	time perl matchip_testgroup.pl
