#!/bin/sh
./evtool.py purge /tmp/test.db
(seq -f "foo:%g" 50000;
seq -f "bar:%g" 50000;
seq -f "baz:%g" 50000;) \
| time ./evtool.py update /tmp/test.db -
./evtool.py fetch /tmp/test.db | wc -l
