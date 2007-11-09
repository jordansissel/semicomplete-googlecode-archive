#!/bin/sh

tmp=$(mktemp -d)
if [ ! -d "$tmp" ] ; then
  echo "$tmp is not a directory"
  exit 1
fi

geom="602x250"
title_secs="4"
blur_secs="2"
frame_rate="25"
last_frame=0
source="halo3logo.jpg"
prefix="intro_frame"
BC="bc -lq"

title() {
  frames="$(echo "$title_secs * $frame_rate" | $BC)"
  echo "title frames: $frames"
  cp halo3logo.jpg $tmp
  cd $tmp
  for i in $(seq $frames); do
    output="$tmp/intro_frame_${last_frame}.jpg"
    ln halo3logo.jpg $output > /dev/null 2>&1
    last_frame="$(($last_frame + 1))"
  done
}

fade() {
  frames="$(echo "$blur_secs * $frame_rate" | $BC)"
  echo "Blur/fade frames: $frames"
  for i in $(seq $frames); do
    output="$tmp/intro_frame_${last_frame}.jpg"
    pct="$(echo "$i / $frames * 100" | $BC)%"
    composite black.jpg $source -blend $pct $output
    mogrify -blur $pct $output
    last_frame="$(($last_frame + 1))"
  done
}

encode() {
  mencoder mf://$tmp/intro_frame_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc \
    -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o intro.avi
}

title
fade
encode
