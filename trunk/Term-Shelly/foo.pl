#!/usr/bin/perl

use Shelly;

my @funcs = qw(help hop server connect login join logout);

my $sh = Term::Shelly->new();

$sh->{"completion_function"} = \&completer;

my @COMMANDS = qw(help server login connect message disconnect hop);
my @WORDS = qw(hello how are you today fantasy rumble forward down fast);

while (1) {
	$sh->do_one_loop();
}

sub completer {
	my ($line, $bword, $pos, $curword) = @_;

	my @matches;

	# Context-sensitive completion.
	#
	# Only complete commands if our current word begins on the 0th column
	# and starts with a / (slash)
	if (($bword == 0) && (substr($line,$bword,1) eq '/')) {
		# We want to complete a command...
		$word = substr($curword,1);
		@matches = map { "/$_" } grep(m/^\Q$word\E/i, @COMMANDS);
	} else {
		@matches = grep(m/\Q$curword\E/i, @WORDS);
	}

	return @matches;
}
