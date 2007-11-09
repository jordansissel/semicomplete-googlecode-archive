#!/bin/sh

tmp=$1
if [ ! -d "$tmp" ] ; then
  echo "$tmp is not a directory"
  exit 1
fi

geom="602x250"
frame_rate="25"
last_frame=0
source="halo3logo.jpg"
prefix="intro_frame"
BC="bc -lq"

title() {
  title_secs=${1:-3}
  frames="$(echo "$title_secs * $frame_rate" | $BC)"
  echo "title frames: $frames"
  cp $source $tmp
  for i in $(seq $frames); do
    f="$(printf "%020d" $(($last_frame + $i)))"
    output="$tmp/intro_frame_${f}.jpg"
    ln $tmp/halo3logo.jpg $output
  done
  last_frame="$(($last_frame + $frames))"
}

fade() {
  fade_secs=${1:-2}
  frames="$(echo "$fade_secs * $frame_rate" | $BC)"
  echo "fade frames: $frames"
  cp $source $tmp
  seq $frames \
  | xargs -P4 -t -n1 sh fadetoblack.sh $source $frames $last_frame $tmp
  last_frame="$(($last_frame + $frames))"
}

encode() {
  mencoder mf://$tmp/intro_frame_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc \
    -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o intro.avi
}

title 3 && \
fade 3 && \
true

rm $tmp/$(basename $source) > /dev/null 2>&1 

#if echo "$tmp" | grep -q '^/tmp'; then
  ##rm -rf $tmp
#else
  #echo "Not cleaning up. tmp is not in /tmp? $tmp"
#fi
