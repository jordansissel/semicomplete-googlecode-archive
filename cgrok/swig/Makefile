
_grok.so: grok_wrap.o
	g++ -shared grok_wrap.o _grok.so

grok_wrap.o: grok.swig
	swig -c++ -I/usr/include -python -o grok_wrap.cpp grok.swig
	g++ -fPIC -I/usr/include -c grok_wrap.cpp -o grok_wrap.o -I/usr/include/python2.5/
