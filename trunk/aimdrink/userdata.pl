#!/usr/bin/perl

use XML::TreeBuilder;
use XML::Writer;
use vars qw($DEBUG);
use strict;

my $CONFIG = "data.xml";

unless (defined($DEBUG)) {  $DEBUG = 10; }

#
#  <user name="PsiKronic">
#     <Option> Data </Option>
#  </user>
#

if (! -e "data.xml") {
   print "Config file doesn't exist, creating...";
   open(NEWCONFIG,"> data.xml") or die("Failed openning data.xml for writing");
   print NEWCONFIG << "END";
<?xml version="1.0" encoding="ISO-8859-1" ?>

<data>
</data>
END
   close(NEWCONFIG);

   print "done.\n";
}

sub read_users {
    my $userdata = shift;
    my ($tree,@users);

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
	  $userdata->{$user->attr("name")}{$tag->tag()} = $tag->as_text();
       }
    }

    return $userdata;
}

sub write_users {
    my $userdata = shift;

    print "Writing user data to disk...\n" if ($DEBUG >= 1);

    foreach my $user (sort(keys(%{$userdata}))) {
       print "User: $user\n" if ($DEBUG >= 2);
       foreach my $key (keys(%{$userdata->{$user}})) {
	  print "   $key: " . $userdata->{$user}{$key}."\n"
	     if ($DEBUG >= 3);
       }
    }

    print "Done\n" if ($DEBUG >= 1);
}

#my $userdata;
#$userdata = read_users();
#write_users($userdata);
