# $Id$
package Tic::Commands;

use strict;
use Tic::Common;
use POSIX qw(strftime);
use vars qw(@ISA @EXPORT);
use Exporter;

@ISA = qw(Exporter);
@EXPORT = qw(create_alias remove_alias command_msg command_alias
					  command_unalias command_echo command_info command_login
					  command_quit command_buddylist command_default
					  command_undefault command_log command_timestamp command_who
					  command_timestamp command_getaway command_help command_date);

my $state;

sub import {
	#debug("Importing from Tic::Commands");
	Tic::Commands->export_to_level(1,@_);
}

sub set_state {
	my $self = shift;
	#debug("Setting state for ::Commands");
	$state = shift;
	Tic::Common->set_state($state);
}

sub create_alias {
	my ($alias, $cmd) = @_;
	$alias =~ s!^/!!;

	$state->{"aliases"}->{$alias} = $cmd;
}

sub remove_alias {
	my ($alias) = @_;

	undef($state->{"aliases"}->{$alias});
}

sub command_msg {
	return "%s" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /msg <screenname> <message>
HELP

	my $aim = $state->{"aim"};

	$state = shift;

	my ($args) = @_;
	my ($sn, $msg);
  
	if (scalar(@_) == 1) {
		($sn, $msg) = split(/\s/, $args, 2);
	} else {
		($sn, $msg) = @_;
	} 
	error("Message who?") and return unless defined($sn);
	error("You didn't tell me what to say!") and return unless defined($msg);
	
	$aim->send_im($sn, $msg);
	my $wholog = get_config("who_log");
	if ((get_config("logging") eq "all" || ((ref($wholog) eq 'HASH' && $wholog->{"$sn"} == 1)))) {
		prettylog($state,"out_msg", { sn => $sn, msg => $msg } );
	}
	prettyprint($state,"out_msg", { sn => $sn, msg => $msg } );
}

sub command_help {
	return "%c"if ($_[0] eq "completion");
	if ($_[0] eq "help") {
		my $commands = join(" ",map("/$_",sort(keys(%{$state->{"commands"}}))));
		#$commands = join("\n", split(/^.{1,75} /,$commands));
		return << "HELP";
Syntax: /help <command>

Available Commands:
$commands

Features worth knowing about:
 - Tab completion: You can tab-complete aliases, commands, and screennames
   (depending on where they are in the command prompt)
 - Aliases: Tired of typing /msg foobar? Make an alias of it! See /help /alias
HELP
	} # if "help"

	my ($state,$what) = @_;
	$what =~ s/\s.*//;

	$what = "help" unless ($what ne "");
	if (defined($what)) {
		$what =~ s!^/!!;
		my $cmd = $state->{"commands"}->{$what};
		if (defined($cmd)) {
			#print STDERR "CMD: $cmd\n";
			prettyprint($state,"help", { help => &{$cmd}("help") } );
			return;
		}

		my $alias = $state->{"aliases"}->{$what};
		if (defined($alias)) {
			prettyprint($state,"alias_is", { alias => $what, value => $alias });
			return;
		}

		prettyprint($state,"nohelp", { subject => $what });
	}
}

sub command_alias {
	return "%a %c %ca" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /alias <alias> <stuff to alias>
For example, let's say you wanted /p to be aliased to /msg psikronic
   /alias p /msg psikronic
Now typing "/p hey!" will send psikronic a message saying "hey!"
Aliases can be recursive, that is:
  /alias foo /msg
  /alias bar /foo psikronic
  /alias baz /bar Hey!
HELP

	$state = shift;
	my ($args) = @_;
	my ($alias, $cmd) = split(/\s+/, $args, 2);
	my $aliases = $state->{"aliases"};

	if ($alias =~ m/^$/) {
		if (scalar(keys(%{$aliases})) == 0) {
			out("There are no aliases set.");
		} else {
			out("Aliases:");
			foreach my $alias (keys(%{$aliases})) {
				next unless (defined($aliases->{$alias}));
				out("$alias => " . $aliases->{$alias});
			}
		}
		return;
	}

	if ($cmd =~ m/^$/) {
		if (defined($aliases->{$alias})) {
			out("$alias => " . $aliases->{$alias});
		} else {
			error("No such alias, \"$alias\"");
		}
	} else {
		create_alias($alias, $cmd);
	}
}

sub command_unalias {
	return "%a" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /unalias <alias>
This command will remove an alias.
HELP
	$state = shift;
	my ($args) = @_;
	my ($alias) = split(/\s+/, $args);

	if ($alias =~ m/^$/) {
		error("Unalias what?");
		return;
	}

	remove_alias($alias);
	out("Removed the alias \"/$alias\"");
}

sub command_echo {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /echo <string>
This isn't really useful, but whatever. It's obvious what this does.
HELP
	$state = shift;
	my ($args) = @_;
	out($args);
}

sub command_getaway {
	return "%s" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /getaway <screenname>
Grabs the away message of the screenname you specify.
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};
	my $sn = $args;

	if ($sn eq '') {
		error("Invalid number of arguments to /getaway");
		return;
	}
	$aim->get_away($sn);

}

sub command_info {
	return "%s" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /info <screenname>
This will look up the user's profile and display it. If you have lynx installed,
the output will be filtered through it so that html will be pretty.
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};
	my ($sn,$key) = split(/\s+/,$args);

	if ($sn eq '') {
		error("Invalid number of arguments to /info.");
		return;
	}

	if ($key eq '') {
		out("Fetching user info for $sn");
		$aim->get_info($sn);
	} else {
		out("State info for $sn");
		out("$key: " . $aim->buddy($sn)->{$key});
	}
}

sub command_login {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /login
Re-login. Useful if you've been disconnected.
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};
	if ($args eq '-f') {
		login();
	} else {
		if ($aim->is_on()) {
			error("You are already logged in, use /login -f to force reconnection.");
		} else {
			login();
		}
	}
}

sub command_quit {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /quit
Duh.
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};
	error("Bye :)");
	$aim->signoff();
	exit;
}

sub command_buddylist {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /buddylist
Displays your buddy list in a semi-formatted, partially ordered fashion.
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};

	foreach my $g ($aim->groups()) {
		out($g);
		foreach my $b ($aim->buddies($g)) {
			my $bud = $aim->buddy($b,$g);

			my $extra;
			if ($bud) {
				$extra .= " [MOBILE]" if $bud->{mobile};
				$extra .= " [TYPINGSTATUS]" if $bud->{typingstatus};
				$extra .= " [ONLINE]" if $bud->{online};
				$extra .= " [TRIAL]" if $bud->{trial};
				$extra .= " [AOL]" if $bud->{aol};
				$extra .= " [FREE]" if $bud->{free};
				$extra .= " [AWAY]" if $bud->{away};
				$extra .= " {".$bud->{comment}."}" if defined $bud->{comment};
				$extra .= " {{".$bud->{alias}."}}" if defined $bud->{alias};
				$extra .= " (".$bud->{extended_status}.")" if defined $bud->{extended_status};
			}

			out("$b ($extra)");
		}
	}
}

sub command_default {
	return "%s" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /default <screenname>
Sets the default user to send messages do. Once set, you no longer have to type
/msg to send this person a message. You can simply type at the prompt. This is
useful for convenience-sake and for pasting things to someone
HELP
	$state = shift;
	my ($args) = @_;
	my $aim = $state->{"aim"};
	($args) = split(/\s+/,$args);

	if ($args eq '') {
		if ($state->{"default"}) {
			out("Default target: " . $state->{"default"});
		} else {
			error("No default target yet");
		}
	} else {
		if ($aim->buddy("$args")) {
			$state->{"default"} = $args;
			out("New default target: $args");
		} elsif ($args eq ';') {
			if ($state->{"last_from"}) {
				$state->{"default"} = $state->{"last_from"};
				out("New default target: " . $state->{"default"});
			} else {
				error("No one has sent you a message yet... what are you trying to do?!");
			}
		} else {
			error("The buddy $args is not on your buddylist, I won't default to it.");
		}
	}
}

sub command_undefault {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /undefault
This will clear the default target setting.
HELP
	$state = shift;
	my ($args) = @_;
	out("Default target cleared.");
	undef($state->{"default"});
}

sub command_log {
	return "%s" if ($_[0] eq "completion");
	return << "HELP" if ($_[0] eq "help");
Syntax: /log <<+|->screenname>|<on|off>
This has lots of different uses:
/log +                  Turn logging on for *everyone*
/log +screenname        Turn logging on for only this screenname. You can log
                        multiple people at once, this simply adds people to
								the list of ones to log.
/log -screenname        Disable logging for this screenname
/log -                  Completely disable all logging
/log on                 Enable logging (NOT THE SAME AS /log +)
/log off                Same as /log -
HELP
                        
	$state = shift;
	my ($args) = @_;

	if ($args eq "+") {
		set_config("logging", "all");
		out("Now logging all messages.");
	} elsif (($args eq "-") || ($args =~ m/^off$/i)) {
		set_config("logging", "off");
		out("Stopping all logging.");
	} elsif ($args =~ m/^on$/) {
		set_config("logging", "on");
		out("Logging is now on.");
	} elsif ($args eq '') {
		set_config("logging", "off") unless (defined(get_config("logging")));
		my $logstate = get_config("logging");
		out("Logging: $logstate");
		if ($logstate =~ m/^only/) {
			my $who = get_config("who_log");
			my @wholog = grep($who->{$_} == 1, keys(%{$who}));
			out("Currently logging: " . join(", ", @wholog));
		}
	} else {
		set_config("logging", "only specified users");
		set_config("who_log", {}) unless defined(get_config("who_log"));

		foreach (split(/\s+/,$args)) {
			if (m/^-(.*)/) {
				get_config("who_log")->{$1} = undef;
				out("Stopped logging $1");
			} elsif (m/^\+?(.+)/) {
				get_config("who_log")->{$1} = 1;
				out("Logging for $1 started");
			}
		}
	}

}

sub command_timestamp {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /timestamp on|off
Turns timestamping of output on or off.
HELP

	$state = shift;
	my ($args) = @_;
	out("command_timestamp($state,$args)");

	if ($args =~ m/^(yes|on|plz)$/i) {
		$state->{"timestamp"} = 1;
		prettyprint($state, "generic_status", { msg => "Timestamps are now on." } );
	} elsif ($args =~ m/^(no|off)$/i) {
		$state->{"timestamp"} = 0;
		prettyprint($state, "generic_status", { msg => "Timestamps are now off." } );
	} elsif ($args =~ m/^$/) {
		my $status = ( ($state->{"timestamp"}) ? "on" : "off" );
		prettyprint($state, "generic_status", { msg => "Timestamps are $status." } );
	} else {
		prettyprint($state, "error_generic", "Invalid parameter to /timestamp");
	}
}

sub command_date {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /date
Display's a timestamp.
HELP
	out("Time: " . strftime(get_config("timestamp"),localtime(time)));

}

sub command_who {
	return "" if ($_[0] eq 'completion');
	return << "HELP" if ($_[0] eq "help");
Syntax: /who
Displays your buddy list
HELP
	$state = shift;

	my ($args) = @_;
	my $aim = $state->{"aim"};

	foreach my $g ($aim->groups()) {
		out($g);
		my @buddies = $aim->buddies($g);
		my $count = 0;
		foreach my $b (sort(compare($a,$b),@buddies)) {
			my $bud = $aim->buddy($b,$g);
			next unless (defined($bud));
			my $e;
			$e .= "offline" unless ($bud->{"online"});
			$e ||= "online, ";
			$e .= "away, " if ($bud->{"away"});
			$e .= "idle, " if ($bud->{"idle"});
			$e =~ s/, $//;

			if ($args eq '-active') {
				if ($e eq "online") {
					out("   $b ($e)");
					$count++;
				}
			} elsif ($args eq '-idle') {
				if ($e =~ m/idle/) {
					out("   $b ($e)");
				  	$count++;
				}
			} elsif ($args eq '-away') {
				if ($e =~ m/away/) {
					out("   $b ($e)");
				  	$count++;
				}
			} elsif ($args eq '') {
				out("   $b ($e)");
				$count++;
			} else {
				if ($b =~ m/$args/i) {
					out("   $b ($e)");
					$count++;
				}
			}

			#if (($args =~ m/\b$type\b/) && ($e =~ m/\b$type\b/));
		}
		error("No buddies matching your query, '$args'") if ($count == 0);
	}
}

sub compare {
	local $a = buddyscore($a);
	local $b = buddyscore($b);
	return $a <=> $b;
}

sub buddyscore {
	my $buddy = shift;
	$buddy = $state->{"aim"}->buddy($buddy);
	my $sum = 0;
	return -11 if (!defined($buddy));

	$sum += 10 if ($buddy->{online});
	$sum -= 10 unless ($buddy->{online}); 
	$sum -= 5 if ($buddy->{away});
	$sum -= 3 if ($buddy->{idle});
	#out($buddy->{"screenname"} . " = $sum");

	return $sum;
}

1;
