#!/bin/sh

DISPLAY=:1
export DISPLAY

xdpyinfo > /dev/null 2>&1 \
|| (nohup startx -- `which Xvfb` :1 &)

xulrunner application.ini "$@"
