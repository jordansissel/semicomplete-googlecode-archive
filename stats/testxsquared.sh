#!/usr/bin/zsh

base="/tmp/xsquared.db"
rm -rf $base

key="xsquared"
./evtool.py create $base
./evtool.py addrule $base $key dy rate 60 time
./evtool.py addrule $base dy ddy rate 60 time

start=`python -c 'import time; print time.time()'`

SIZE=10000
seq 0 $SIZE | awk '{printf("'$key'@%ld:%ld\n", $1*1000000, $1^2)}' \
| python ./evtool.py update $base -

end=`python -c 'import time; print time.time()'`
echo "Writes per second:"
echo "$SIZE / ($end - $start)" | bc -lq
