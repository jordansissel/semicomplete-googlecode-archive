
start=1160611100
rrd="/tmp/my.rrd"
rm $rrd
rrdtool create $rrd --start $start --step 86400 \
  DS:humans:GAUGE:86400:0:U \
  RRA:LAST:.5:1:500

./evtool.py fetch /tmp/webhits.db2 agg.human.daily \
| awk '{print "update","'$rrd'","-t humans",($2/1000000)":"$3}' \
| sort \
| rrdtool -

rrdtool graph hits.rrd.png -s $start \
  DEF:humans=$rrd:humans:LAST \
  LINE1:humans#338800:"human hits"
