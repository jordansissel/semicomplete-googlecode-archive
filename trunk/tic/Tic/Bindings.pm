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

	"^I"          =>    "complete-word",
	"TAB"         =>    "complete-word",

	"BACKSPACE"   =>    "delete-char-backward",
	"^H"          =>    "delete-char-backward",
	"^?"          =>    "delete-char-backward",
	"^U"          =>    "kill-line",
};

# Default "string" -> sub binding.
use constant DEFAULT_MAPPINGS => {
	"backward-char"           => \&backward_char,
	"forward-char"            => \&forward_char,
	"complete-word"           => \&complete_word,
	"delete-char-backward"    => \&delete_char_backward,
	"kill-line"               => \&kill_line,

};

my $state;

sub import {
	#debug("Importing from Tic::Bindings");
	Tic::Bindings->export_to_level(1,@_);
}

sub set_state {
	my $self = shift;
	$state = shift;
}

sub prepare_completion {
	#my ($state,$commands) = @_;
	my $commands = shift;

	while (my ($key,$val) = each(%{$commands})) {
		$state->{"completion"}->{$key} = &{$val}("completion");
		out("Completion: $key = " . $state->{"completion"}->{$key});
	}
}

sub forward_char {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} < length($state->{"input_line"})) {
		$state->{"input_position"}++;
		#my $pos = length($state->{"input_line"});
		$ret->{-print} = "\e[C";
	}

	return ($state->{"input_string"}, undef, $ret);
}

sub backward_char {
	my $state = shift;
	my $ret;
	if ($state->{"input_position"} > 0) {
		$state->{"input_position"}--;
		$ret->{-print} = "\e[D";
	}

	return ($state->{"input_string"}, undef, $ret);
}

sub kill_line {
	my $state = shift;
	my $ret;
	$ret->{-print} = "\e[2K\r";
	my ($line) = $state->{"input_line"};
	$line = ""; # not undef...
	$state->{"input_position"} = 0;
	$state->{"input_line"} = undef;
	return (undef, undef, { -print => "\r\e[2K" } );  # Not undef!
}

sub complete_word {
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

	for (my $x = 0; $x < length($line); $x++) {
		unless (defined($b)) { # Look for the beginning of the word
			my $p = $pos - $x;
			#print STDERR "B: $p / '" . substr($line,$p,1) . "'\n";
			$b = $p if (($p <= 0) || (substr($line,$p,1) =~ m/^\s/));
			$b++ if ($b > 0);
		}
		unless (defined($e)) { # Look for the end of the word
			my $p = $pos + $x;
			#print STDERR "F: $p / '" . substr($line,$p,1) . "'\n";
			$e = $p if (($p >= length($line)) || (substr($line,$p,1) =~ m/^\s/));
		}
		last if (defined($b) && defined($e));
	}
	$b ||= 0;
	$e ||= length($line);

	#print STDERR "Init: $pos / Beginning: $b / End: $e == '" . substr($line,$b,$e) . "'\n";

	$string = substr($line,$b,($e - $b));
	my $comp = do_complete($state, $string, $b, $e);
	if ($comp ne $string) {
		substr($line,$b,($e - $b)) = $comp;
		$state->{"input_position"} += length($comp) - length($string);

	}

	#print STDERR "Line: $line\n";

	return ($line, undef);
}

sub do_complete {
	my ($state,$string,$b,$e) = @_;
	if ($b eq '0') {
		if ($string =~ m!^/(.*)$!) {
			$string = do_complete_command($state,$1);
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
		print STDERR join(" / ", @compl). "\n";
		print STDERR "How: $how / $index / $string / $cmd / " . $state->{"completion"}->{$cmd} . "\n";

		FOO:
		$state->{"recursion_check"}++;
		if ($state->{"recursion_check"} > 10) {
			$state->{"recursion_check"} = 0;
			return $string;
		}
		if ($how eq "%a") {
			$string = do_complete_alias($state,$string);
		} elsif ($how eq "%c") {
			$string = do_complete_command($state,substr($string,1)) if ($string =~ m!^/!);
		} elsif ($how eq "%s") {
			$string = do_complete_buddy($state,$string);
		} elsif ($how eq "%ca") {
			# look back, find the command.
			print STDERR "Looking back ($index)\n";
			for (my $i = $index; $i >= 0; $i--) {
				print STDERR "$i | $words[$i]\n";
				if ($words[$i] =~ m!^/(.*)!) {
					print "{".$words[$i] . "} / ";
					my $c = $1;
					if (defined($state->{"commands"}->{$c})) {
						my @compl = split(" ",$state->{"completion"}->{$c});
						$how = $compl[$index - $i - 1];
						print STDERR "----\n";
						print STDERR "%CA: $c : $how / $index - $i\n";
						print STDERR join(" / ", @compl) . "\n";
						print STDERR "----\n";
						goto FOO;
					}
				}
			}
		}
	}

	$state->{"recursion_check"} = 0;
	return $string;
}

sub do_complete_command {
	my ($state, $partial) = @_;
	my @coms = grep(m/^$partial/,keys(%{$state->{"commands"}}));
	push(@coms,grep(m/^$partial/,keys(%{$state->{"aliases"}})));
	@coms = sort(@coms);
	$partial  = $coms[0] . " " if (scalar(@coms) > 0);
	return "/$partial";
}

sub do_complete_alias {
	my ($state, $partial)  = @_;
	print STDERR "alias completion, $partial\n";
	my @coms = grep(m/^$partial/,keys(%{$state->{"aliases"}}));
	@coms = sort(@coms);
	$partial  = $coms[0] . " " if (scalar(@coms) > 0);
	return $partial;
}

sub do_complete_buddy {
	my ($state, $partial) = @_;

	my @matches;
	foreach my $group ($state->{"aim"}->groups()) {
		my @buddies = $state->{"aim"}->buddies($group);
		print STDERR "$group - " . scalar(@buddies) . " / " . scalar(grep(m/^$partial/,@buddies)) . " / $partial\n";
		push(@matches, grep(m/^$partial/i,@buddies));
	}

	$partial = $matches[0] . " " if (scalar(@matches) > 0);

	return $partial;
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
