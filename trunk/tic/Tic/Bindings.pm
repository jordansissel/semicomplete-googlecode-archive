package Tic::Bindings;

#use Tic::Shenanigans

use Exporter;

# Default key -> sub bindings
use constant DEFAULT_BINDINGS => {
	"LEFT"        =>    "bind_left",
	"RIGHT"       =>    "bind_right",

	"^I"          =>    "bind_complete",
	"TAB"         =>    "bind_complete",

	"BACKSPACE"   =>    "bind_backspace",
	"^H"          =>    "bind_backspace",
	"^U"          =>    "bind_killline",
};

# Default "string" -> sub binding.
use constant DEFAULT_MAPPINGS => {
	"bind_left"         => \&bind_left,
	"bind_right"        => \&bind_right,
	"bind_complete"     => \&bind_complete,
	"bind_backspace"    => \&bind_backspace,
	"bind_killline"     => \&bind_killline,

};

@ISA = qw(Exporter);
@EXPORT = qw(DEFAULT_BINDINGS DEFAULT_MAPPINGS);

sub import {
	print STDERR "Importing... " . join("\n", @_) . "\n";
	Tic::Bindings->export_to_level(1,@_);
}

sub bind_right {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} < length($state->{"input_line"})) {
		$state->{"input_position"}++;
		#my $pos = length($state->{"input_line"});
		$ret->{-print} = "\e[C";
	}

	return ($state->{"input_string"}, undef, $ret);
}

sub bind_left {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} > 0) {
		$state->{"input_position"}--;
		$ret->{-print} = "\e[D";
	}

	return ($state->{"input_string"}, undef, $ret);
}

sub bind_killline {
	my $state = shift;
	my $ret;
	$ret->{-print} = "\e[2K\r";
	my ($line) = $state->{"input_line"};
	$line = ""; # not undef...
	$state->{"input_position"} = 0;
	$state->{"input_line"} = undef;
	return (undef, undef, { -print => "\r\e[2K" } );  # Not undef!
}

sub bind_complete {
	my $state = shift;
	my $line = $state->{"input_line"};
	my $pos = $state->{"input_position"};
	my $ret;
	my $string;
	#print STDERR "bind_complete called.\n";

	# Try completing a command.
	# Find the word the cursor is on.
	my ($b,$e); # beginning and end
	$pos-- if (substr($line,$pos,1) eq ' ');

	for ($x = 0; $x < length($line); $x++) {
		unless (defined($b)) { # Look for the beginning of the word
			my $p = $pos - $x;
			print STDERR "B: $p / '" . substr($line,$p,1) . "'\n";
			$b = $p if (($p <= 0) || (substr($line,$p,1) =~ m/^\s/));
		}
		unless (defined($e)) { # Look for the end of the word
			my $p = $pos + $x;
			print STDERR "F: $p / '" . substr($line,$p,1) . "'\n";
			$e = $p if (($p >= length($line)) || (substr($line,$p,1) =~ m/^\s/));
		}
		last if (defined($b) && defined($e));
	}
	$b ||= 0;
	$e ||= length($line);

	print STDERR "Init: $pos / Beginning: $b / End: $e == '" . substr($line,$b,$e) . "'\n";

	$string = substr($line,$b,($e - $b));
	my $comp = do_complete($state, $string, $b, $e);
	if ($comp ne $string) {
		substr($line,$b,($e - $b)) = $comp;
		$state->{"input_position"} += length($comp) - length($string);

	}

	print STDERR "Line: $line\n";

	return ($line, undef);
}

sub do_complete {
	my ($state,$string,$b,$e) = @_;
	if ($b eq '0') {
		if ($string =~ m!^/(.*)!) {
			# Try to complete a command.
			print STDERR "Commands: " . join("\n", keys(%{$state->{"commands"}}))."\n";
			@coms = grep(m/^$1/,keys(%{$state->{"commands"}}));
			push(@coms,grep(m/^$1/,keys(%{$state->{"aliases"}})));
			print STDERR "Matches: " . scalar(@coms) . "\n";
			@coms = sort(@coms);
			if (scalar(@coms) > 0) {
				$string = "/" . $coms[0] . " ";
			}
		}
	}
	return $string;
}

sub bind_backspace {
	my $state = shift;
	my $ret;
	my ($line) = $state->{"input_line"};
	if ($state->{"input_position"} > 0) {
		#if ($state->{"input_position")
		print STDERR "Backspace!\n";
		my ($pos,$pos2) = (length($line), $state->{input_position});
		$line = substr($line, 0, $state->{"input_position"} - 1) .
				  substr($line, $state->{"input_position"});
		$state->{"input_position"}--;
		#print STDERR "End/Pos -> $pos/$pos2\n";

		# go from pos -> end of string + 1, print a space, jump back to pos.
		$ret->{-print} =  "\e[".($pos - $pos2 - 2)."C \e[".($pos-$pos2-1)."D";
	} else {
		print STDERR "No Backspace!\n";
	}
	return ($line,undef,$ret);
}

# true dat...
1;
