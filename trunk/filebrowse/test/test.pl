#!/usr/bin/perl

$path = "/home/group/s99/redbull0/code/alex.tar/alex.jpg";

if ( $path =~ /^(.*?(\.tar\.gz|\.tar|\.tgz))\/(.*?)$/i ) {
  print "$1 $2 $3\n";
  print "asdf";
}
