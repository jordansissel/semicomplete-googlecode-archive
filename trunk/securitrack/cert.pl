#!/usr/bin/perl

use strict;
use warnings;

use WWW::Mechanize;
use HTML::TreeBuilder;
use HTTP::Date;
use XML::Simple;

my $mech = WWW::Mechanize->new();
my $tree = HTML::TreeBuilder->new();

$mech->get("http://www.kb.cert.org/vuls/byupdate?open&count=1000");

if ($mech->status() != 200) {
	croak("HTTP Status is is not OK, was: " . $mech->status() . "\n");
} else {
	print STDERR "Request for CERT vulnerabilities successful.\n";
}


#$te->parse($mech->content());
$tree->parse($mech->content());

#@0.1.1.0.2.0

my $foo = $tree->content()->[1]->content()->[1]->content()->[0]->content()->[2]->content()->[0];

my $line = 0;
my $data;

foreach my $row ($foo->look_down(_tag => "tr")) {
	next unless scalar(@{$row->content()}) == 3;
	$line++;
	next if $line == 1;

	my ($date, $id, $name) = map { $_->content()->[0] } @{$row->content()};

	# fudge date
	$date =~ s!(\d{2})/(\d{2})/(\d{4}) (\d{2}):(\d{2}):(\d{2}) (AM|PM)!$3-$1-$2 $4:$5:$6!;
	if ($7 eq 'AM') {
		$foo = (($4 == 12) ? 0 : $4 + 12);
		$date =~ s/(\S+) (\d{2}):/$1 $foo:/;
	}

	#my $strp = new DateTime::Format::Strptime( pattern => "%Y-%m-%d %T" , on_error => 'croak');

	my $entry = {
	               date => [ str2time($date) ],
						id =>  $id->content()->[0],
						url => [ "http://www.kb.cert.org/vuls/" . $id->attr('href') ],
						descr => [ $name ],
	};

	push(@{$data->{vulnerability}}, $entry);

}

print XMLout($data, RootName => "cert");
