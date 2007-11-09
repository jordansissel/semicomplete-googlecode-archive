#!/bin/sh

geom="602x250"
title_secs="4"
blur_secs="2"
frame_rate="25"
last_frame=0
source="halo3logo.jpg"

prefix="intro_frame"

BC="bc -lq"

frames="$(echo "$title_secs * $frame_rate" | $BC)"
echo "title frames: $frames"
for i in $(seq $frames); do
  ln halo3logo.jpg intro_frame_${last_frame}.jpg > /dev/null 2>&1
  last_frame="$(($last_frame + 1))"
done

frames="$(echo "$blur_secs * $frame_rate" | $BC)"
echo "Blur/fade frames: $frames"
for i in $(seq $frames); do
  output="intro_frame_${last_frame}.jpg"
  pct="$(echo "$i / $frames" | $BC)"
  composite black.jpg $source -blend $pct  $output
  mogrify -blur $pct $output
  last_frame="$(($last_frame + 1))"
done
