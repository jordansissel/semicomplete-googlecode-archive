#!/usr/bin/perl

=pod

=head1 NAME

Term::Shelly - Yet Another Shell Kit for Perl

=head1 GOAL

I needed a nonblocking shell kit for an aim client I was writing. All of the Term::ReadLine modules are blocking in doing their readline() functions, and as such are entirely unacceptable. This module is an effort on my part to provide the advanced functionality of great ReadLine modules like Zoid into a package that's more flexible, extendable, and most importantly, nonblocking reads to allow other things to happen a tthe same time.

=head1 NEEDS

- Settable key bindings
- Tab completion
- Support for window size changes (sigwinch)
- movement in-line editing.
- vi mode (Yeah, I lub vi)
- history
- Completion function calls

- Settable callbacks for when we have an end-of-line (EOL binding?)
=cut

package Term::Shelly;

use strict;
use warnings;

use IO::Handle; # I need flush();

use Term::ReadKey;


my %KEY_CONSTANTS = (
							"\e[A"      => "UP",
							"\e[B"      => "DOWN",
							"\e[C"      => "RIGHT",
							"\e[D"      => "LEFT",
						  );

ReadMode 3;

my $state = { };

($state->{"termcols"}) = GetTerminalSize();
$SIG{WINCH} = sub { ($state->{"termcols"}) = GetTerminalSize(); };

my $bindings = { 
	"LEFT"        => "backward-char",
	"RIGHT"       => "forward-char",

	"BACKSPACE"   => "delete-char-backward",
	"^H"          => "delete-char-backward",
	"^?"          => "delete-char-backward",
	
	"^U"          => "kill-line",
	
	"^J"          => "newline",
	"^M"          => "newline",

	"^A"          => "beginning-of-line",
	"^E"          => "end-of-line",
	"^K"          => "kill-to-eol",
	"^L"          => "redraw",

	#"^I"          => "complete-word",
	#"TAB"         => "complete_word",

	#"^T"          => "expand-line",
};

my $mappings = { 
	"backward-char"          => \&backward_char,
	"forward-char"           => \&forward_char,
	"delete-char-backward"   => \&delete_char_backward,
	"kill-line"              => \&kill_line,
	"newline"                => \&newline,
	"redraw"                 => \&fix_inputline,
	"beginning-of-line"      => \&beginning_of_line,
	"end-of-line"            => \&end_of_line,

	#"complete-word"          => \&complete_word,
	#"expand-line"            => \&expand_line,
};

sub init {
	$state->{"input_line"} = "";
	$state->{"input_position"} = 0;
	$state->{"leftcol"} = 0;
}

# Nonblocking readline
sub do_one_loop { 
	my $char;

	# ReadKey(-1) means no timeout waiting for data, thus is nonblocking
	while (defined($char = ReadKey(-1))) {
		handle_key($char);
	}
	
}

=pod

=over 4

=item handle_key($key)

Handle a single character input. This is not a "key press" so much as doing all the necessary things to handle key presses.

=cut

sub handle_key($) {
	my $char = shift;

	my $line = $state->{"input_line"} || "";
	my $pos = $state->{"input_position"} || 0;

	if ($state->{"escape"}) {
		$state->{"escape_string"} .= $char;
		if ($state->{"escape_expect_ansi"}) {
			$state->{"escape_expect_ansi"} = 0 if ($char =~ m/[a-zA-Z]/);
		}

		$state->{"escape_expect_ansi"} = 1 if ($char eq '[');
		$state->{"escape"} = 0 unless ($state->{"escape_expect_ansi"});

		unless ($state->{"escape_expect_ansi"}) {
			my $estring = $state->{"escape_string"};

			$state->{"escape_string"} = undef;
			return execute_binding("\e".$estring);
		}

		return 0;
	}

	if ($char eq "\e") {      # Trap escapes, they're speshul.
		$state->{"escape"} = 1;
		$state->{"escape_string"} = undef;
		
		# What now?
		return 0;
	}

	if ((ord($char) < 32) || (ord($char) > 126)) {   # Control character
		execute_binding($char);
		return 0;
	}

	if ((defined($char)) && (ord($char) >= 32)) {
		substr($line, $pos, 0) = $char;
		$state->{"input_position"}++;
	}

	$state->{"input_line"} = $line;
	fix_inputline();
}

sub execute_binding {
	my $str = shift;
	my $key = prettify_key($str);

	#print "Key: $key\n";
	if (defined($bindings->{$key})) {
		if (ref($mappings->{$bindings->{$key}}) eq 'CODE') {
			return &{$mappings->{$bindings->{$key}}};
		} else {
			error("Unimplemented function, " . $bindings->{$key});
		}
	}

	return 0;
}


sub prettify_key ($) {
	my $key = shift;

	# Return ^X for control characters, like CTRL+A...
	if (length($key) == 1) {   # One-character keycombos should only be ctrl keys
		if (ord($key) <= 26) {  # Control codes, another check anyway...
			return "^" . chr(65 + ord($key) - 1);
		}
		if (ord($key) == 127) { # Speshul backspace key
			return "^?";
		}
		if (ord($key) < 32) {
			return "^" . (split("", "\]_^"))[ord($key) - 28];
		}
	}

	# Return ESC-X for escape shenanigans, like ESC-W
	if (length($key) == 2) {
		my ($p, $k) = split("", $key);
		if ($p eq "\e") {    # This should always be an escape, but.. check anyway
			return "ESC-" . $k;
		}
	}

	# Ok, so it's not ^X or ESC-X, it's gotta be some ansi funk.
	return $KEY_CONSTANTS{$key};
}

sub real_out {
	print @_;
}

sub out {
	real_out(@_, "\n");
	fix_inputline();
}

sub error { 
	print STDERR "*> ", @_, "\n";
}

sub fix_inputline {
	print "\r\e[2K";

	if ($state->{"input_position"} - $state->{"leftcol"} > $state->{"termcols"}) {
		$state->{"leftcol"} += 30;
	}

	# only print as much as we can in this one line.
	print substr($state->{"input_line"}, $state->{"leftcol"}, $state->{"termcols"});;
	print "\r";
	print "\e[" . ($state->{"input_position"} - $state->{"leftcol"}) . 
	      "C" if ($state->{"input_position"} > 0);
	STDOUT->flush();
}

sub newline {
	# Process the input line.

	real_out("\n");
	print "You wrote: " . $state->{"input_line"} . "\n";

	$state->{"input_line"} = undef;
	$state->{"input_position"} = 0;
}

sub kill_line {
	$state->{"input_line"} = undef;
	$state->{"input_position"} = 0;

	real_out("\r\e[2K");

	# Fix input line?

	return 0;
}

sub forward_char {
	if ($state->{"input_position"} < length($state->{"input_line"})) {
		$state->{"input_position"}++;
		real_out("\e[C");
	}
}

sub backward_char {
	if ($state->{"input_position"} > 0) {
		$state->{"input_position"}--;
		real_out("\e[D");
	}
}

sub delete_char_backward {
	#"delete-char-backward"   => \&delete_char_backward,
	if ($state->{"input_position"} > 0) {
		substr($state->{"input_line"}, $state->{"input_position"} - 1, 1) = '';
		$state->{"input_position"}--;

		if ($state->{"input_position"} - $state->{"leftcol"} <= 0) {
			$state->{"leftcol"} -= 30;
			$state->{"leftcol"} = 0 if ($state->{"leftcol"} < 0);
		}

		fix_inputline();
	}
}

sub beginning_of_line {
	$state->{"input_position"} = 0;
	$state->{"leftcol"} = 0;
	fix_inputline();
}

sub end_of_line {
	$state->{"input_position"} = length($state->{"input_line"});
	fix_inputline();
}
# Test method...
init;

while (1) {
	do_one_loop();
}

1;
