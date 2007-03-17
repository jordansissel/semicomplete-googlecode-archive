#!/usr/bin/perl
#
# ******** OBEY ROBOTS.TXT ***********8

use warnings;
use strict;

use DBI;

my $BASEDIR = "/spider";

{ package Foo::WWW::Mechanize;
	use base 'WWW::Mechanize';

	sub redirect_ok {
		my $self = shift;
		my $prospective_request = shift;
		my $response = shift;

		my $ok = $self->SUPER::redirect_ok($prospective_request, $response);
		#print "Redirect ok? $prospective_request\n";
		
		return $ok;
	}
}

my $site = $ARGV[0];
my $db = DBI->connect("dbi:Pg:dbname=spider","spider");

print STDERR "No website specified\n" and exit(1) unless (defined($site));

$site = "http://".$site unless ($site =~ m!^http://!);
my %done;

$site =~ m!([a-zA-Z]+://)([^/]+)(/.*)?!;
my $dirbase = $2;
my $sitebase = $1 . $2;
mkdir($BASEDIR . "/" . $dirbase);

my $mech = Foo::WWW::Mechanize->new( agent => "OddBott/1.0");
my ($pages, $total);

$site .= "/" if ($site =~ m!^([a-zA-Z]+)://[^/]+$!);

print "Site: $site\n";

my @links;
push(@links,{ url => $site, from => "" });

while (scalar(@links)) {
	follow_links(shift @links);
}

sub follow_links {
	my ($url,$from) = shift;
	$from = $url->{from};
	$url = $url->{url};
	my $realurl = $url;
	$url =~ s!#[^\?]+!!;
	return if (defined($done{$url}));
	#return if ($url =~ m/^(mailto|javascript):/);
	#print "NOT " if (defined($done{$url}));
	if ($url =~ m/(^.*)\?(.*)$/) {
		if (defined($done{$1}) && $done{$1} >= 5) {
			return;
		}
		$done{$1}++;
	} else {
		$done{$url}++;
	}
	$url =~ m/(^.*)\?(.*)$/;

	$mech->head($url);
	return unless ($mech->status() == 200);# || $mech->status() == 302);
	return unless ($mech->response()->header("Content-Type") =~ m!^text/!);
	#if ($mech->response()->header("Content-Type")

	print "\rVisiting: [". ($done{ $1 || $url } || 0) ."] $url\n";
	$mech->get($url);
	$pages++;

	save_page($url);

	$total+=length($mech->content());
	print "\tSize: " . length($mech->content()) . " | Total: $total bytes / $pages pages\n";

	my $a = 0;
	my $links = $mech->links();
	foreach my $link (@{$links}) {
		my $url = $link->url_abs();
		
		if ($url =~ m!\.(?:zip|mpe?g|mp3|mov|tar|gz|tgz|bz2|tbz|wav)$!) {
			#print STDERR "$url is not a pagez0r...\n";
			next;
		}

		my $res = $db->prepare("SELECT lastupdate FROM sites WHERE url=?");
		my $u = $url->as_string();
		$res->execute($u);
		if ($res->rows() == 0) {
			my $time = time();
			$time = 0 if ( ($url =~ m!^[a-zA-Z]+:/!) && ($url !~ m!^$site!) );
			$res = $db->prepare("INSERT INTO sites (url,lastupdate) VALUES(?,?)");
			$res->execute($u,$time);
			$res->finish();
		} else {
			my $time = time();
			$time = 0 if ( ($url =~ m!^[a-zA-Z]+:/!) && ($url !~ m!^$site!) );
			$res = $db->prepare("UPDATE sites SET lastupdate=? WHERE url=?");
			$res->execute(time(),$u);
			$res->finish();
		}

		if ( ($url =~ m!^[a-zA-Z]+:/!) && ($url !~ m!^$site!) ) {
			print STDERR "$url is off-site. [$site]\n";
			next;
		}

		push(@links,{ url => $u, from => $realurl } );
		#follow_links($url);
	}
}

sub save_page {
	my ($url) = shift;
	my ($host,$uri,$file) = ("UNKNOWN","UNKNOWN","UNKNOWN");
	

	$url =~ m![a-zA-Z]+://([^/]+)/(.*)$!;
	$host = $1;
	$uri = $2 || "";

	if ($uri !~ m!^[a-zA-Z]+:/!) {
		$uri = "$dirbase/$uri";
	}
	#print "\tURI: $uri\n";

	if ($uri !~ m!(?:(?:/\?)|(?:\.[^/]{3,4}(?:$|(?:\?.*))$))!) {
		$file = "index.html";
	} else {
		$uri =~ m!^(.*)/([^/]+)$!;
		$uri = $1;
		$file = $2;
	}


	#print "\tHost: $host\n";
	#print "\tDirectory: $BASEDIR/$uri\n";
	#print "\tFile: $file\n";
	print "\tSaving to -> $BASEDIR/$uri/$file\n";

	system("mkdir -p $BASEDIR/$uri");
	open(WWW, "> $BASEDIR/$uri/$file") or die("open: Failed trying to open: $BASEDIR/$uri/$file\nReason:$!\n");
	print WWW $mech->content();
	close(WWW);
}
