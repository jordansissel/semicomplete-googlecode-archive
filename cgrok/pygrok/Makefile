
all:
	python setup.py build

test:
	cd build/lib.*; \
	python -c 'import pygrok; gre = pygrok.GrokRegex("H.*"); print gre.search("Hello")'

clean:
		rm build/*/*.so || true
		rm build/*/*.o || true
