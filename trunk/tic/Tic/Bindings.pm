# $Id$
package Tic::Bindings;

use strict;
use Tic::Common;
use Tic::Commands;
use Net::OSCAR qw/:standard/;
use vars ('@ISA', '@EXPORT');
use Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(DEFAULT_BINDINGS DEFAULT_MAPPINGS prepare_completion);

my $state;
my $sh;

sub import {
	#debug("Importing from Tic::Bindings");
	Tic::Bindings->export_to_level(1,@_);
}

sub set_state {
	my $self = shift;
	$state = shift;
	Tic::Common->set_state($state);
	$sh = $state->{"sh"};

	$sh->{"bindings"}->{"^T"} = "expand-line";
	$sh->{"bindings"}->{";"} = "complete-lastfrom";
	$sh->{"mappings"}->{"expand-line"} = \&expand_line;
	$sh->{"mappings"}->{"complete-lastfrom"} = \&complete_lastfrom;

	#$sh->out("Ref: " . ref($sh->{"mappings"}->{"kill-line"}));
	push(@{$sh->{"mappings"}->{"kill-line"}}, \&killline_binding);
	$sh->{"completion_function"} = \&completer;
	$sh->{"anykey_callback"} = \&anykey_binding
}

sub prepare_completion {
	#my ($state,$commands) = @_;
	my $commands = shift;

	while (my ($key,$val) = each(%{$commands})) {
		$state->{"completion"}->{$key} = &{$val}("completion");
		#out("Completion: $key = " . $state->{"completion"}->{$key});
	}
}

sub expand_line {
   my $string = $sh->{"input_line"};
   $sh->{"input_line"} = expand_aliases($string);
   $sh->{"input_position"} = length($sh->{"input_line"});
   $sh->fix_inputline();
}

sub anykey_binding {
	my $line = $sh->{"input_line"};
	my $aim = $state->{"aim"};
	my $sn;

	$line = expand_aliases($line);

	#$sh->out("Line: '$line'");

	if (length($line) == 0) {
		killline_binding();
		return;
	}

	# Let the user know we're typing...

	if ($line =~ s!^/msg\s+!!) {
		$sn = next_arg(\$line);

		return unless defined($aim->buddy($sn));

		# We're typing to someone on our buddylist
		if (length($line) > 0) {
			prompter("TARGET", $sn);

			# Drop off the first 2 arguments.. this is a lame hack
			$line = \$sh->{"input_line"};
			my $chopped = next_arg($line);
			$chopped .= next_arg($line);
			$line =~ s/^\s//;
			$sh->{"input_position"} = 0;
			#$sh->out("New line: " . $$line . " / " . $sh->{"input_position"} . " / $chopped");

			$state->{"target"} = $sn;
			$sh->fix_inputline();
		}

		return unless $aim->buddy($sn)->{"typing_status"} == 1;

		#$sh->out("Target: $sn\nLine: '$line'");
	} elsif ($line !~ m!^/!) { # line starts with something that isn't /
		$sn = $state->{"target"} || $state->{"default"}
	}

	# Record that we've typed...
	if ($aim->is_on) {
		if ($state->{"typing_status"}->{"status"} != TYPINGSTATUS_STARTED) {
			$aim->send_typing_status($sn, TYPINGSTATUS_STARTED);
			$state->{"typing_status"} = {
				"status" => TYPINGSTATUS_STARTED,
				"sn" => $sn,
				"time" => time,
			};
		}

		$state->{"idle"} = time();
		if ($state->{"is_idle"} == 1) {
			$sh->out("Setting not-idle");
			$state->{"is_idle"} = 0;
			$aim->set_idle(0);
		}
	}
}

sub killline_binding {
	if (defined($state->{"typing_status"})) {
		$state->{"aim"}->send_typing_status($state->{"typing_status"}->{"sn"}, 
														TYPINGSTATUS_FINISHED);
		delete $state->{"typing_status"};
	}
}

sub complete_lastfrom {
   if ($sh->{"input_position"} == 0) {
      # We just hit the ';' key, and we're at the beginning of the line.

      if (defined($state->{"last_from"})) {
         $sh->insert_at_cursor("/msg \"" . $state->{"last_from"} . "\" ");
      } else {
         $sh->error("No one's messaged you yet :(");
      }
   } else {
      $sh->insert_at_cursor(";");
   }
} 

# Called by Term::Shelly's complete-word
sub completer {
	my ($line, $bword, $pos, $curword) = @_;

	my @matches;
	if (($bword == 0) && (substr($line, $bword, 1) eq '/')) {
		# Complete a command.
		my @coms = keys(%{$state->{"commands"}});
		push(@coms, keys(%{$state->{"aliases"}}));
		@coms = sort(@coms);

		my $word = substr($curword, 1);
		@matches = map { "/$_" } grep (m/^\Q$word\E/i, @coms);
	} else {

		# Complete screennames for certain commands
		if ($line =~ m!^/(?:
								(?:m(?:sg)?)|        # /m or /msg
								(?:delbuddy)|        # /delbuddy
								(?:default)|         # /default
								(?:i(?:nfo)?)|       # /i or /info
								(?:log)|              # /log
								(?:getaway)|
							  )\s+!x) {
			@matches = match_buddies($curword);
		}
	}

	return @matches;
}

sub match_buddies {
	my $word = shift;

	$word =~ s/^"(.*)"?$/$1/;

	return map { '"' . ($state->{"buddylist"}->{"$_"}->{"sn"} || $_) . '"' } grep(m/^\Q$word\E/i, keys(%{$state->{"buddylist"}}));
}

1;
