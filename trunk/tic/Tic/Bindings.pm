# $Id$
package Tic::Bindings;

use strict;
use Tic::Common;
use vars ('@ISA', '@EXPORT');
use Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(DEFAULT_BINDINGS DEFAULT_MAPPINGS prepare_completion);

# Default key -> sub bindings
use constant DEFAULT_BINDINGS => {
	"LEFT"        =>    "backward-char",
	"RIGHT"       =>    "forward-char",

	#"^I"          =>    "complete-word",
	#"TAB"         =>    "complete-word",

	"BACKSPACE"   =>    "delete-char-backward",
	"^H"          =>    "delete-char-backward",
	"^?"          =>    "delete-char-backward",
	"^U"          =>    "kill-line",

	"^T"          =>    "expand-line",
};

# Default "string" -> sub binding.
use constant DEFAULT_MAPPINGS => {
	"backward-char"           => \&backward_char,
	"forward-char"            => \&forward_char,
	"complete-word"           => \&complete_word,
	"delete-char-backward"    => \&delete_char_backward,
	"kill-line"               => \&kill_line,
	"expand-line"             => \&expand_line
};

my $state;

sub import {
	#debug("Importing from Tic::Bindings");
	Tic::Bindings->export_to_level(1,@_);
}

sub set_state {
	my $self = shift;
	$state = shift;
	Tic::Common->set_state($state);
}

sub prepare_completion {
	#my ($state,$commands) = @_;
	my $commands = shift;

	while (my ($key,$val) = each(%{$commands})) {
		$state->{"completion"}->{$key} = &{$val}("completion");
		#out("Completion: $key = " . $state->{"completion"}->{$key});
	}
}

sub forward_char {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} < length($state->{"input_line"})) {
		$state->{"input_position"}++;
		#my $pos = length($state->{"input_line"});
		if ($state->{"input_position"} > ($state->{"termcols"} + $state->{"leftcol"})) {
			$state->{"leftcol"}++;
		}
		$ret->{-print} = "\e[C";
	}

	return ($state->{"input_line"}, undef, $ret);
}

sub backward_char {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} > 0) {
		$state->{"input_position"}--;
		if (($state->{"input_position"} <= $state->{"leftcol"}) && ($state->{"input_position"} != 0)) {
			$state->{"leftcol"}--;
		}
		$ret->{-print} = "\e[D";
	} 
	print STDERR "Back: " . $state->{"leftcol"} . " / " . $state->{"input_position"} . "\n";

	return ($state->{"input_line"}, undef, $ret);
}

sub kill_line {
	my $state = shift;
	my $ret;
	$ret->{-print} = "\e[2K\r";
	my ($line) = $state->{"input_line"};
	$line = ""; # not undef...
	$state->{"input_position"} = 0;
	$state->{"leftcol"} = 0;
	$state->{"input_line"} = undef;
	return (undef, undef, { -print => "\r\e[2K" } );  # Not undef!
}

sub expand_line {
	my $state = shift;
	my $line = $state->{"input_line"};
	$line = expand_aliases($line);
	$state->{"input_position"} = length($line);

	return ($line,undef,undef);
}

sub complete_word {
	my $state = shift;
	my $line = $state->{"input_line"};
	my $pos = $state->{"input_position"};
	my $ret;
	my $string;

	$pos = $state->{"complete_position"} if (defined($state->{"complete_position"}));
	$line = $state->{"complete_oldline"} if (defined($state->{"complete_oldline"}));

	# Try completing a command.
	# Find the word the cursor is on.
	my ($b,$e); # beginning and end

	#if (!defined($state->{"complete_position"})) {
		$pos-- if (substr($line,$pos,1) eq ' ');

		for (my $x = 0; $x < length($line); $x++) {
			unless (defined($b)) { # Look for the beginning of the word
				my $p = $pos - $x;
				$b = $p if (($p <= 0) || (substr($line,$p,1) =~ m/^\s/));
				$b++ if ($b > 0);
			}
			unless (defined($e)) { # Look for the end of the word
				my $p = $pos + $x;
				$e = $p if (($p >= length($line)) || (substr($line,$p,1) =~ m/^\s/));
			}
			last if (defined($b) && defined($e));
		}
		$b ||= 0;
		$e ||= length($line);
		$string = substr($line,$b,($e - $b));
	#} else {
		$string = $state->{"complete_partial"} if (defined($state->{"complete_partial"}));;
	#}

	print STDERR "$b:$e - [$string] $line\n";

	my $movepos = $state->{"complete_beginpos"} || -1;
	my $comp = do_complete($state, $string, $b, $e);
	if ($comp ne $string) {
		substr($line,$b,($e - $b)) = $comp;
		#$state->{"input_position"} -= ($e - $b);
		$state->{"input_position"} = $b;
		$state->{"input_position"} += length($comp);
		#$state->{"complete_lastcomp"} = $comp;
		#$state->{"input_position"} += length($comp) - length($string);
	}

	#print STDERR "Line: $line\n";

	return ($line, undef);
}

sub do_complete {
	my ($state,$string,$b,$e) = @_;
	my $orig = $string;
	my $matches;

	$state->{"complete_oldline"} = $state->{"input_line"} 
		unless defined($state->{"complete_oldline"});

	if ($b eq '0') {
		if ($string =~ m!^/(.*)$!) {
			($string, $matches) = do_complete_command($state,$1);
		}
	} else {
		# This isn't the first word, and maybe isn't a command.
		return if ($string eq '');
		my $line = $state->{"input_line"};
		my @words = split(/\s+/,$line);

		my $cmd = substr($words[0],1);
		my @compl = split(" ",$state->{"completion"}->{$cmd});
		my $idx = 0;
		my $index;
		foreach my $word (@words) {
			$index = $idx if ($string eq $word);
			$idx++;
			last if ($index);
		}

		my $how = $compl[$index - 1];

		FOO:
		$state->{"recursion_check"}++;
		if ($state->{"recursion_check"} > 10) {
			$state->{"recursion_check"} = 0;
			return $string;
		}
		if ($how eq "%a") {
			($string, $matches) = do_complete_alias($state,$string);
		} elsif ($how eq "%c") {
			($string, $matches) = do_complete_command($state,substr($string,1)) if ($string =~ m!^/!);
		} elsif ($how eq "%s") {
			($string, $matches) = do_complete_buddy($state,$string);
		} elsif ($how eq "%ca") {
			# look back, find the command.
			#print STDERR "Looking back ($index)\n";
			for (my $i = $index; $i >= 0; $i--) {
				#print #STDERR "$i | $words[$i]\n";
				if ($words[$i] =~ m!^/(.*)!) {
					print "{".$words[$i] . "} / ";
					my $c = $1;
					if (defined($state->{"commands"}->{$c})) {
						my @compl = split(" ",$state->{"completion"}->{$c});
						$how = $compl[$index - $i - 1];
						goto FOO;
					}
				}
			}
		}
	}

	if ($string ne $orig) {
		print STDERR "Orig: $orig / New: $string\n";
		print STDERR "--> " . $state->{"complete_index"} . "\n";
		print STDERR "--> " . $state->{"input_position"} . "\n";
		$state->{"complete_position"} = $state->{"input_position"};
		$state->{"complete_partial"} = $orig;
		$state->{"complete_index"} = (($state->{"complete_index"} + 1) % 
												scalar(@{$matches}));
		$state->{"complete_beginpos"} = $b;
		$state->{"complete_endpos"} = $e;
	}
	$state->{"recursion_check"} = 0;
	return $string;
}

sub do_complete_command {
	my ($state, $partial) = @_;
	my @coms = grep(m/^$partial/,keys(%{$state->{"commands"}}));
	push(@coms,grep(m/^$partial/,keys(%{$state->{"aliases"}})));
	@coms = sort(@coms);
	$partial  = $coms[$state->{"complete_index"}] . " " if (scalar(@coms) >= 0);
	return "/$partial", \@coms;
}

sub do_complete_alias {
	my ($state, $partial)  = @_;
	#print STDERR "alias completion, $partial\n";
	my @coms = grep(m/^$partial/,keys(%{$state->{"aliases"}}));
	@coms = sort(@coms);
	$partial  = $coms[$state->{"complete_index"}] . " " if (scalar(@coms) > 0);
	return $partial, \@coms;
}

sub do_complete_buddy {
	my ($state, $partial) = @_;

	my @matches;
	foreach my $group ($state->{"aim"}->groups()) {
		my @buddies = $state->{"aim"}->buddies($group);
		#print STDERR "$group - " . scalar(@buddies) . " / " . scalar(grep(m/^$partial/,@buddies)) . " / $partial\n";
		push(@matches, grep(m/^$partial/i,@buddies));
	}

	$partial = $matches[$state->{"complete_index"}] . " " if (scalar(@matches) > 0);

	return $partial, \@matches;
}

sub delete_char_backward {
	my $state = shift;
	my $ret;
	my ($line) = $state->{"input_line"};
	if ($state->{"input_position"} > 0) {
		my ($pos,$pos2) = (length($line), $state->{input_position});
		#$line = substr($line, 0, $state->{"input_position"} - 1) .
				  #substr($line, $state->{"input_position"});

		substr($line,$state->{input_position} - 1,1) = "";

		$state->{"input_position"}--;

		# go from pos -> end of string + 1, print a space, jump back to pos.
		$ret->{-print} =  "\e[".($pos - $pos2 - 2)."C \e[".($pos-$pos2-1)."D";
	} 
	return ($line,undef,$ret);
}

1;
