#!/usr/bin/perl -w

use File;
use strict;

require "Commands.pl";

my $path = $ARGV[ 0 ] || "~";

foreach my $file ( lscmd( $path ) ) {
  print "$file->{file}\t$file->{size}\t$file->{mode}\t$file->{type}\n";
  if ( $file->{mp3} ) {
    print "$file->{mp3}->{artist}\n";	
  }
}
