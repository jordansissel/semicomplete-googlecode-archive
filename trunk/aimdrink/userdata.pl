#!/usr/bin/perl

package Drink;

use IO;
use XML::TreeBuilder;
use XML::Writer;
use vars qw($DEBUG $CONFIG);
use strict;

my $CONFIG = "data.xml";

$DEBUG = 3;

#
#  <user name="PsiKronic">
#     <Option> Data </Option>
#  </user>
#

if (! -e $CONFIG) {
   print "Config file doesn't exist, creating...";
   open(NEWCONFIG,"> $CONFIG") or die("Failed opening $CONFIG for writing");
   print NEWCONFIG "<data>\n</data>";
   close(NEWCONFIG);
   print "done.\n";
}

sub read_users {
    my ($tree,@users);
    my $userhash = {};

    $tree = new XML::TreeBuilder();
    $tree->parse_file($CONFIG);

    @users = $tree->look_down('_tag','user');

    foreach my $user (@users) {
       print "Name: " .$user->attr("name")."\n" if ($DEBUG >= 2);
       my @a = $user->content_list();
       foreach my $tag ($user->content_list()) {
	  next unless ref($tag); #Ignore empty shit.
	  print "<".$tag->tag() . ">: " . $tag->as_text() ."\n"
	     if ($DEBUG >= 2);
	  $userhash->{$user->attr("name")}{$tag->tag()} = $tag->as_text();
       }
    }

    return $userhash;
}

sub find_user {
   my ($userhash,$user) = @_;

   print "Finding user, $user | $userhash\n";
   unless (defined($userhash->{$user})) {
      #Unknown user, create a default 
      $userhash->{$user} = { 'language' => "default" };
      print "Def: " . defined($userhash->{$user}) . "\n";
   }
   print "Userhash: " .\{$userhash->{$user}}."\n";
   return $userhash->{$user};
}

sub write_users {
   my $userhash = shift;

   print "Writing user data to disk...\n" if ($DEBUG >= 1);
   print ": $userhash\n";

   my $out = new IO::File(">$CONFIG");
   my $o = new XML::Writer(OUTPUT => $out, DATA_MODE => 1, DATA_INDENT => 3);

   $o->startTag("data");

   print ": ". keys(%{$userhash}) ."\n";
   foreach my $user (sort(keys(%{$userhash}))) {
      print "User: $user\n" if ($DEBUG >= 2);
      $o->startTag("user", "name" => "$user");
      foreach my $key (keys(%{$userhash->{$user}})) {
	 print "   $key: " . $userhash->{$user}{$key}."\n" if ($DEBUG >= 3);
	 $o->startTag("$key");
	 $o->characters($userhash->{$user}{$key});
	 $o->endTag("$key");
      }
      $o->endTag("user");
   }

   $o->endTag("data");
   $o->end();
   $out->close();

}
1;
