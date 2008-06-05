#!/bin/sh

export RANGE=40
export OFFSET=`expr 255 - $RANGE`

# This perl code generates variables $BG and $FG.
# BG is a random dark color, and $FG is a light version of $BG.
eval $(perl -le 'sub c { int(rand() * $ENV{RANGE} + $ENV{OFFSET}) }; sub p { printf("%s=#%02x%02x%02x\n", @_) }; @f = (c, c, c); @b = map($_ - $ENV{OFFSET}, @f);  p("FG",@f); p("BG", @b);')

[ $# -eq 0 ] && set -- screen -RR
STY= exec xterm -fg white -ms "$FG" -bg "$BG" -fn '*suxus*' -cr green -e "$@"
