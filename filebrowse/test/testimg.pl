#!/usr/bin/perl -w
use Image::Size;
use strict;

my ( $x, $y ) = imgsize($ARGV[0]);
print "Dimensions: $x x $y\n";
