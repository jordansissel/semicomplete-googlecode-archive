#!/usr/bin/perl

use strict;
use warnings;

use DBI;
use Proc::ProcessTable

my $t = Proc::ProcessTable;

foreach my $p (@{$t->table}) {
	my $pid = $p->pid;
	my $cmd = $p->cmndline;

	if ($cmd =~ m/spider.pl/) {
		print "$pid - $cmd\n";
	}
}

my $db = DBI->connect("dbi:Pg:dbname=spider","spider");




