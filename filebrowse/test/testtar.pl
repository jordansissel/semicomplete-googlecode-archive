#!/usr/bin/perl -w
use Archive::Tar;
use File::Type;
use strict;

my $tar = Archive::Tar->new();
$tar->read($ARGV[0], 1);
my @files = $tar->list_files;

my $ft = File::Type->new();

foreach my $file ( @files ) {
  print "File: $file\n";
  print "Type: " . $ft->checktype_contents( $tar->get_content( $file ) ) . "\n";
}  
