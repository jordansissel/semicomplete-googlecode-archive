#!/usr/bin/perl -w
use File::MimeInfo;
use strict;

my @files = glob("*");

foreach my $file ( @files ) {
	my $type = mimetype( $file );
	if ( $type ) {
	  print "$file :: $type\n";
	}
}
