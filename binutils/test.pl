#!/usr/bin/perl

use IPC::Open2;

open2($read,$write,"/usr/local/libexec/nmh/mhl");

while (<>) {
   chomp();
   print ">>> $_\n";
   print $write $_ . "\n";

   last if (m/^$/);
}
print ">>> " . <$read> . "\n";

close($read); close($write);
