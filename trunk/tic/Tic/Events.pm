package Tic::Events;

use strict;
use Tic::Common;
use Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw( event_admin_error          event_chat_closed
						event_admin_ok             event_chat_im_in
						event_buddy_in             event_connection_changed
						event_buddy_info           event_error
						event_buddy_out            event_evil
						event_buddylist_error      event_im_in
						event_buddylist_ok         event_im_ok
						event_chat_buddy_in        event_rate_alert
						event_chat_buddy_out       event_signon_done );

my $state;

sub import {
	debug("Importing from Tic::Events");
	Tic::Events->export_to_level(1,@_);
}

sub set_state {
	$state = shift;
}

sub event_admin_error {
	my ($aim, $reqtype, $error, $errurl) = @_;
}

sub event_admin_ok {
	my ($aim, $reqtype) = @_;
}

sub event_buddy_in {
	my ($aim, $sn, $group, $data) = @_;
	if (!defined($state->{"buddylist"}->{$sn})) {
		out("* $sn ($group) is online.");
	} else {
		# Here we can add sections to output data
		# pertainting to the user's new state.
	}
	$state->{"buddylist"}->{$sn} = $data;

}

sub event_buddy_info {
	my ($aim, $sn, $data) = @_;
	foreach (@{$state->{"hooks"}->{"buddy_info"}}) {
		if ($sn eq $_->{sn}) {
			&{$_->{sub}}($sn);
			return;
		}
	}

	if ($data->{"profile"}) {
		out("Buddy info for $sn");
		out("------------------");
		if (1) { # If they have w3m...
			my $prof = $data->{'profile'};
			out(`echo "$prof" | lynx -dump -stdin`);
			#out(`echo "$prof" | w3m -T text/html -dump`);
		}
	} elsif ($data->{"awaymsg"}) {
	}

}

sub event_buddy_out {
	my ($aim, $sn, $group) = @_;
}

sub event_buddylist_error {
	my ($aim, $error, $what) = @_;
}

sub event_buddylist_ok {
	my ($aim) = @_;
}

sub event_chat_buddy_in {
	my ($aim, $sn, $chat, $data) = @_;
}

sub event_chat_buddy_out {
	my ($aim, $sn, $chat) = @_;
}

sub event_chat_closed {
	my ($aim, $chat, $error) = @_;
}

sub event_chat_im_in {
	my ($aim, $from, $chat, $msg) = @_;
}

sub event_connection_changed {
	my ($aim, $conn, $status) = @_;
}

sub event_error {
	my ($aim, $conn, $err, $desc, $fatal) = @_;
	error("$desc");
	if ($fatal) {
		error("This error was fatal and you have been disconnected. :(") ;
		$aim = Net::OSCAR->new();
		$state->{"signon"} = 0;
	}
	
}

sub event_evil {
	my ($aim, $newevil, $from) = @_; 
}

sub event_im_in {
	my ($aim, $from, $msg, $away) = @_;
	$state->{"last_from"} = $from;
	out("<*$from*> $msg");
}

sub event_im_ok {
	my ($aim, $to, $reqid) = @_;
}

sub event_rate_alert {
	my ($aim, $level, $clear, $window, $worrisome) = @_;
}

sub event_signon_done {
	my ($aim) = @_;
	$state->{"signon"} = 0;
}

1;
