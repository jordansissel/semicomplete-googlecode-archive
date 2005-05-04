#!/usr/bin/perl
# 
# Check an email for message/rfc822 mime parts
# If it exists, look at the Received: headers for our mail server.
# If our mail server is not in the Received: list, then the mail
# obviously wasn't from us, so it shouldn't have been bounced 
# to us.

use strict;
use MIME::Tools;
use MIME::Parser;

my $parser = new MIME::Parser;
$parser->output_to_core('ALL');
$parser->extract_nested_messages("NEST");
#$parser->ignore_errors(1);

my $ent = $parser->parse_open("-");

#print "Ent: " . $ent->mime_type . "\n";
#print "Ent: " . $ent->parts . "\n";

my $isfromcsh = 0;
foreach my $part ($ent->parts) {
	#print "Ent: " . $part->mime_type . "\n";

	if ($part->mime_type() eq 'message/rfc822') {
		foreach (@{$part->body}) {
			last unless (m/^(?:\s|Received:)/);
			$isfromcsh = 1 if (m/from \w+.csh.rit.edu/);
		}
	}
}
print "isfromcsh: $isfromcsh\n";
exit 0;
