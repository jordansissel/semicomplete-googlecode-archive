# $Id$
package Tic::Bindings;

use strict;
use Tic::Common;
use Tic::Commands;
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

	$sh->{"completion_function"} = \&completer;
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
