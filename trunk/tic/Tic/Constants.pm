package Tic::Constants;

use Exporter;
@ISA = qw(Exporter);

@EXPORT qw(%KEY_CONSTANTS)

my %KEY_CONSTANTS {
	"\e[A"    =>     "UP",
	"\e[D"    =>     "LEFT"
	"\e[C"    =>     "RIGHT",
	"\e[D"    =>     "DOWN",
}

