#!/usr/bin/perl

use Cvs;

print $ENV{CVSROOT};
my $cvs = new Cvs ( cvsroot => $ENV{ CVSROOT } );

print "Modules";
my @modules = $cvs->module_list();
foreach my $module ( @modules ) {
  print $module;
}
