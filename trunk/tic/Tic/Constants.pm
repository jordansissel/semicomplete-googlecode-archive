package Tic::Constants;

use Exporter;
use constant KEY_CONSTANTS => {
		"\e[A"    =>     "UP",
		"\e[B"    =>     "DOWN",
		"\e[C"    =>     "RIGHT",
		"\e[D"    =>     "LEFT",
	};


@ISA = qw(Exporter);
@EXPORT_OK = qw(KEY_CONSTANTS);


sub import {
	Tic::Constants->export_to_level(1,@_);
}

# true dat...
1;
