
all:
	python setup.py build

test: all
	echo "1234 5000 15243" \
	| python example.py "%NUMBER>5000%" \
	| grep -q 5000 \
	&& echo "Python test passed."

clean:
		rm build/*/*.so || true
		rm build/*/*.o || true
