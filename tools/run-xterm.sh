#!/bin/sh

export RANGE=40
export OFFSET=`expr 255 - $RANGE`

#FG=`perl -e 'sub f { sprintf("%02x", int(rand() * $ENV{RANGE} + $ENV{OFFSET})) }; print "#".f.f.f'`
#BG="#"`perl -e 'sub b { printf("%02x", (hex(shift) - $ENV{OFFSET}) * 1.2)}; $noop while ($ARGV[0] =~ m/([0-9A-Fa-f]{2})(?{b($^N)})/g);' "${FG#?}"`

eval $(perl -le 'sub c { int(rand() * $ENV{RANGE} + $ENV{OFFSET}) }; sub p { printf("%s=#%02x%02x%02x\n", @_) }; @f = (c, c, c); @b = map($_ - $ENV{OFFSET}, @f);  p("FG",@f); p("BG", @b);')

[ $# -eq 0 ] && set -- screen -RR
#STY= exec xterm -fg "$FG" -ms "$FG" -bg "$BG" -fn '*suxus*' -cr green -e "$@"
LANG=C STY= exec xterm -fg white -ms "$FG" -bg "$BG" -fn '*suxus*' -cr green -e "$@"
#STY= exec rxvt +sb -fg "$FG" -pr "$FG" -bg "$BG" -fn '*suxus*' -cr green -e "$@"
