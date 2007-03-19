#!/bin/sh

netstat_poll() {
 netstat -s \
 | awk '/^[a-z]*:/ { type = substr($1,1,length($1)-1); } 
        /^	[^	]/ && $1 ~ /^[0-9]+$/ && $1 > 0 { 
         x=$1; 
         $1=""; 
         gsub(" ","_",$0); 
         print "C_" type $0 "=" x 
       }' \
  | grep -v '^.*[0-9].*=' \
  | tr '\n' '&' \
  | sed -e 's,^,http://home.semicomplete.com/~jls/stats/updater.py?,; ' \
  | xargs -0 -n1 wget -q -O /dev/null
}

update() {
  while true; do
    time=`perl -le 'print time()'`
    netstat_poll

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
