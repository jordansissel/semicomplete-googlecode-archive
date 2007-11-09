#!/bin/sh

source=$1; shift;
frames=$1; shift;
last_frame=$1; shift;
tmp=$1; shift;
step=$1; shift;

pct="$(echo "$step / $frames * 100" | bc -lq)"
cur_frame="$(($last_frame + $step))"
cur_frame="$(printf "%020d" $cur_frame)"

output="$tmp/intro_frame_${cur_frame}.jpg"
composite black.jpg $source -blend ${pct}% $output

#pct="$(echo "$pct / 4" | bc -lq)%"
#mogrify -blur "${pct}%x${pct}%" $output

