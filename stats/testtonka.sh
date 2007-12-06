#!/usr/bin/zsh

base="/tmp/tonkatcp"
rm -vrf $base

./evtool.py create $base
./evtool.py addrule $base tcpOutDataBytes tcp.tonka.rate rate 300 time
./evtool.py addrule $base tcp.tonka.rate tcp.tonka.rate.avg average 3600 time

start=`python -c 'import time; print time.time()'`
file="sample"

awk -F: '{print $1"000000:"$2}' tonka.tcpdata2 | sed -e '$d' > $file
echo 'Starting'
cat $file | time python ./evtool.py update $base -
end=`python -c 'import time; print time.time()'`
count=`wc -l < $file`
echo "Writes per second:"
echo "$count / ($end - $start)" | bc -lq
