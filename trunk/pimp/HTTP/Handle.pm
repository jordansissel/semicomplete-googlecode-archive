=pod

=head1 NAME

HTTP::Handle - HTTP Class designed for streaming

=head1 SYNOPSIS

use HTTP::Handle;

my $http = HTTP::Handle->new( uri => "http://www.google.com/" );
$http->connect();
my $fd = $http->fd();

while (<$fd>) {
	print "--> $_";
}

=head1 DESCRIPTION

Revision: $Id$

The C<HTTP::Handle> module allows you to make HTTP requests and handle
the data yourself. The general ideas is that you use this module to make
a HTTP request and handle non-header data yourself. I needed such a 
feature for my mp3 player to listen to icecast streams.

=cut

package HTTP::Handle;

use IO::Handle;
use Socket;
use URI;

=pod

=head2 HTTP::Handle->new();

Create a new HTTP::Handle object thingy.

=cut

sub new {
	my $class = shift;

	my $self = {
		# Defaults here
		useragent         => "HTTP-Handle/$VERSION",
	};

	my %args = @_;

	while (my ($key, $val) = each(%args)) {
		$self->{$key} = $val;
	}

	bless $self, $class;

	return $self;
}

=pod 

=head2 $http->connect()

Connect, send the http request, and process the response headers.

=cut

sub connect($) {
	my $self = shift;

	my $sock;

	socket($sock, PF_INET, SOCK_STREAM, getprotobyname('tcp')) or _fatal("Failed creating socket.");

	$self->{"socket"} = $sock;

	_debug("Connecting to " . $self->{"host"} . ":" . $self->{"port"});
	connect($sock, sockaddr_in($self->{"port"}, inet_aton($self->{"host"}))) or _fatal("Failed connecting to " . $self->{"host"} . ":" . $self->{"port"});
	_debug("Connected");

	$sock->autoflush(1);
	print $sock $self->http_request_string();

	chomp($self->{"code"} = <$sock>);
	
	print "Code: " . $self->{"code"} . "\n";

	while (<$sock>) {
		chomp();
		s/\015//;
		last if (m/^$/);
		m/(\S+):\s*(.+)\s*/;
		$self->{"http_response"}->{$1} = $2;
	}
}

=pod

=head2 $http->fd()

Get the file descriptor (socket) we're using to connect.

=cut

sub fd($) {
	my $self = shift;

	return $self->{"socket"};
}

sub url($;$) {
	my $self = shift;
	my $url = shift;

	if (defined($url)) {
		my $uri = URI->new($url);

		$self->{"host"} = $uri->host();
		$self->{"http_path"} = $uri->path();
		$self->{"port"} = $uri->port();
		$self->{"uri"} = $uri;

		$self->{"http_request"}->{"Host"} = $self->{"host"};
		$self->{"http_action"} |= "GET";

	} else {
		return $self->{"url"};
	}
}

sub http_request_string($) {
	my $self = shift;

	my $ret = sprintf("%s %s HTTP/1.0\n", $self->{"http_action"}, $self->{"http_path"}); 
	map { $ret .= sprintf("%s: %s\n", $_, $self->{"http_request"}->{$_}) } keys(%{$self->{"http_request"}});

	$ret .= "\n\n";

	return $ret;
}

sub _fatal {
	print STDERR @_, "\n", "ERR: $!\n";
	exit(1);
}

sub _debug {
	print STDERR map { "$_\n" } @_;
}
1;
