#!/usr/bin/perl

use Net::Ping;
#use IPC::Open2;


my ($x) = 0;

for ($x; $x < 256; $x++) {
   my $ip = "129.21.60." . $x;
   my $p = Net::Ping->new("tcp",1);
   if ($p->ping($ip)) {
      open(SSH,"telnet $ip 22 |");
      <SSH>; chomp();
      print "$ip: $_\n";
      close(SSH);
   } else {
      print "$ip not reachable\n";
   }
   $p->close();
}
