#!/usr/bin/perl

use strict;
use warnings;
use Image::Magick;
use POSIX qw(strftime);

system("mencoder mf://input_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell:autoaspect=1 -oac copy -o output.avi");
