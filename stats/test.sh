#!/usr/bin/zsh

base="/tmp/webhits.db2"
rm -rf $base

./evtool.py create $base
for i in google human yahoo msn twiceler; do
  ./evtool.py addrule $base agent.$i agg.$i.hourly total 3600 time
  ./evtool.py addrule $base agg.$i.hourly agg.$i.daily total 86400 time
  ./evtool.py addrule $base agg.$i.daily agg.$i.weekly_avg average 604800 time
done
start=`python -c 'import time; print time.time()'`
file="sample"
head -100000 webhits.uniqc > $file
#cp webhits.uniqc $file
echo 'Starting'
cat $file | time ./evtool.py update $base -
end=`python -c 'import time; print time.time()'`
count=`wc -l < $file`
echo "Writes per second:"
echo "$count / ($end - $start)" | bc -lq
