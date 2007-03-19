#!/bin/sh

collector="localhost"

update() {
  while true; do
    time=`perl -le 'print time()'`
    (vmstat -s \
     | tr -d '&;' \
     | gawk '$1 ~ /^[0-9][0-9]*$/ {a=$1; $1=""; gsub(" ","_",$0); print "C_"$0"="a}' \
     | sed -e 's/__*/_/;'\
     | tr -d '()[]{}' \
     | tr '\n' '&' ; echo) \
     | sed -e 's,^,'$collector'?,' \
     | xargs -n1 wget -q -O /dev/null

    duration=`perl -le "print 50 - (time() - $time)"`
    sleep $duration
  done
}

if [ "$1" = "start" ] ; then
  update
else
  pkill -9 -f "$0 start"
  pkill -9 -f "$0 start"
  pkill -9 -f "$0 start"
  exec 0> /dev/null
  exec 1> /dev/null
  exec 2> /dev/null
  (nohup $0 start) &
fi
