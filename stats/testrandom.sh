#!/usr/bin/zsh

base="/tmp/awkrandom.db"
key="random"
SIZE=10999

create() {
  rm -rf $base
  ./evtool.py create $base
  ./evtool.py addrule $base $key avg average 101 time
  ./evtool.py addrule $base $key stddev stddev 101 time
  ./evtool.py addrule $base $key variance variance 101 time
  ./evtool.py addrule $base stddev avg_stddev1000 average 1000 time
  ./evtool.py addrule $base variance avg_var1000 average 1000 time
}

gawk_rand() {
  seq 0 $SIZE \
  | gawk 'BEGIN{srand(0)} {printf("'$key'@%ld:%f\n", $1*1000000, rand())}'
}

perl_rand() {
  KEY=$key SIZE=$SIZE perl -le 'for (1..$ENV{SIZE}) { printf("%s@%ld:%f\n", $ENV{KEY}, $_ * 1000000, rand()) }'
}

for i in perl_rand gawk_rand; do
  create
  start=`python -c 'import time; print time.time()'`
  $i | python ./evtool.py update $base -
  end=`python -c 'import time; print time.time()'`
  echo "Writes per second:"
  echo "$SIZE / ($end - $start)" | bc -lq
  ./evtool.py graph /tmp/awkrandom.db avg variance
  cp hits.png hits.$i.png
done

