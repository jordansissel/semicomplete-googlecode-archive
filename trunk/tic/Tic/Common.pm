package Tic::Common;

use strict;
use Exporter;
use Term::ReadKey;

our @ISA = qw(Exporter);
our @EXPORT = qw(out real_out error debug query);

sub import {
	debug("Importing from Tic::Common");
	Tic::Common->export_to_level(1,@_);
}

sub out { real_out(join("\n",@_) . "\n"); }
sub real_out { print @_; }
sub error { print STDERR "*> " . shift() . "\n"; }
sub debug { foreach (@_) { print STDERR "debug> $_\n"; } }

sub query {
   my ($q, $hide) = @_;
   real_out("$q");
   #stty("-echo") if ($hide);
	ReadMode 0 unless ($hide);
   chomp($q = <STDIN>);
   #stty("echo") if ($hide);
   ReadMode 3;
   out() if ($hide);
   return $q;
}

sub stty {
	#return $state->{"stty"} if (!defined($_[0]));
	#$state->{"stty"} = IO::Stty::stty(\*STDIN, "-g");
	return IO::Stty::stty(@_);
}

1;
