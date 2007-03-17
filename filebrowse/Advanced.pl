#!/usr/bin/perl -w

use File::Find;
use strict;

sub findcmd {
  my ( $word, @files ) = @_;

  find( \&wanted, @files );

}

sub wanted {
 /
}
1;
