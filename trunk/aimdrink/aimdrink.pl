#!/usr/bin/perl
#$Id$

use Net::AIM;
use IO::Select;
use Symbol;
use Socket;
use strict;

my $DRINKHOST = "drink.csh.rit.edu";
my $DRINKPORT = 4242;

my %session;
my ($aim,$conn);
my $s = IO::Select->new();

$aim = new Net::AIM;
#$aim->newconn(Screenname => 'CSH Drink', Password => 'cshdrink123');
$aim->newconn(Screenname => 'CSH Drink2', Password => 'cshdrink123');
#$aim->newconn(Screenname => 'mobilesoutherner', Password => 'carleneway');

$aim->debug(1);

#Handlers
$conn = $aim->getconn();
$conn->set_handler("im_in", \&on_im);
$conn->set_handler("error", \&on_error);

my $c = 0;
while (1) {
   $aim->do_one_loop();
   read_from_sessions();

   if ($c == 0) {
      get_drink_stats();
      $c = ($c + 1) % 10;
   }
}

my $frag;
sub read_from_sessions {
   my ($sock,@ready,$line);

   foreach $sock ( $s->can_read(1) ) {
      #print "Ready: " . $sock . "\n";

      my ($key);
      foreach (keys(%session)) {
	 if ($session{$_}->{'socket'} == $sock) {
	    $key = $_;
	 }
      }


      my $brec = recv($sock,$line,8192,0);
      my $data;
      if (defined($brec)) {
	 if (length($line) == 0) { 
	    print "Socket dead, removing...";
	    $aim->send_im($key,"Socket dead.");
	    undef $session{$key};
	    $s->remove($sock); 
	    next;
	 }

	 $data = $frag . $line;
	 my @lines = split(/\n/,$data);
	 chomp($data);
	 $frag = (substr($data,-1) ne "\n" ? pop(@lines) : "");
      }

      print "Drink> $data\n";
      $aim->send_im("$key","Drink> $data");
   }

}


sub on_im {
   my ($hash,$event,$from) = @_;

   my $args = $event->{'args'};

   #foreach (keys(%{$event})) {
   #   print "$_: " . $event->{$_} . "\n";
   #}

   #foreach (@{$args}) { print $_ . "\n"; }
   if ($args->[1] eq 'F') {
      handle_message($from,$args->[2]);
   }

}

sub on_error {
   my ($hash,$event,$from,$to) = @_;
   print STDERR "$from sent error code #".$event->{'args'}->[0] . "\n";
   print STDERR "REF: " . $event->{'args'}."\n";
   print STDERR "--> ";
   my (@b) = @{$event->{'args'}};

   foreach (@b) {
      print "$_ ";
   }
   print STDERR "\n";
}

sub handle_message {
   my ($from,$msg) = @_;

   #Strip HTML:
   $msg =~ s/\<.*?\>//g;

   #print "$from: $msg\n";

   if (defined($session{$from})) {
      send($session{$from}{'socket'},$msg . "\n",0);
   }
   else {
      #New session
      my ($sock, $dconn);

      $sock = Symbol::gensym();

      socket($sock,PF_INET,SOCK_STREAM,getprotobyname('tcp')) 
        or handle_error("Unable to create new socket: $!",1,$from);
      $dconn = connect($sock,sockaddr_in($DRINKPORT, inet_aton($DRINKHOST)));

      $session{$from} = { socket => $sock, conn => $dconn };

      #Selection adding
      $s->add($sock);
      $aim->send_im($from,"Hello new user!");
   }

}

sub handle_error {
   my ($error,$errno,$from) = @_;

   print STDERR "$error";
   if (defined($from)) { $aim->send_im($from,"$error"); }
}

sub get_drink_stats {
   
   print "Fetching stats\n";
   my $sock = Symbol::gensym();

   my $sel = new IO::Select();

   socket($sock,PF_INET,SOCK_STREAM,getprotobyname('tcp')) 
     or handle_error("Unable to create new socket: $!",1);
   connect($sock,sockaddr_in($DRINKPORT, inet_aton($DRINKHOST)))
     or return;

   $sel->add($sock);

   print "Connected to $DRINKHOST:$DRINKPORT.\n";
   send($sock,"stat\n",0);
   print "> $!\n";

   my ($ready,$line,$frag);
   my $info;
   print "Fetching...";
   while (1) {
      ($ready) = $sel->can_read(1);
      last unless (defined($ready));
      my $brec = recv($ready,$line,8192,0);
      my $data;
      print "Line: (".length($line).")" . $line;
      last if (length($line) == 0);
      if (defined($brec)) {

	 $data = $frag . $line;
	 my @lines = split(/\n/,$data);
	 chomp($data);
	 $frag = (substr($data,-1) ne "\n" ? pop(@lines) : "");
      }
      $info .= $data . "\n";
   }
   print "\n";

   print "DONE FETCHING DATA\n";
   $aim->set_info($info);
   print $aim->get_info("CSH Drink");
}
