#!/usr/bin/perl -w
require "Commands.pl";

( $desc, @files ) = view( $ARGV[0] );
print "Description: $desc\n";
foreach $file ( @files ) {
  print "File: $file->{dir} :: $file->{file}\n";
}
