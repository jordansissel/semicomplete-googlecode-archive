#!/bin/sh

try() {
	OUTPUT=`mktemp /tmp/logwatcher.test.XXXXX`
	../logwatcher -f conf/$1 > $OUTPUT 2> /dev/null
	diff -u $OUTPUT output/$1 > /dev/null 2>&1
	return $?
}

TESTS=
if [ $# -gt 0 ]; then 
	TESTS="$@"
else
	TESTS="shellquoting customquoting matchtest"
fi

set -- $TESTS
while [ $# -gt 0 ]; do
	try $1 && echo "Test OK: $1" || echo "Test failed: $1"
	shift
done
