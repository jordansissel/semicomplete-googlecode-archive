use strict;

sub about_cb
{
	my $window = Gtk2::Window->new;

	my $label = Gtk2::Label->new;
	$label->set_markup("Brought to you by:\n<span size='12000' foreground='blue'><b>the SARS\nJordan\nJohn\nLewk\nChris</b></span>\n\n'And there's a module for that.'");

	$window->add($label);
	$window->show_all;
}
sub about_cb_2
{
	my $window = Gtk2::Window->new;
	my $table = Gtk2::Table->new(4, 4, 1);

	$table->attach_defaults(Gtk2::Label->new('Owner'),1,2,0,1);
	$table->attach_defaults(Gtk2::Label->new('Group'),2,3,0,1);
	$table->attach_defaults(Gtk2::Label->new('Other'),3,4,0,1);
	$table->attach_defaults(Gtk2::Label->new('Read'),0,1,1,2);
	$table->attach_defaults(Gtk2::Label->new('Write'),0,1,2,3);
	$table->attach_defaults(Gtk2::Label->new('Execute'),0,1,3,4);

	my @array = [ [ 0, 0, 0 ], [ 0, 0, 0 ], [ 0, 0, 0 ] ];

	for ( my $row = 0; $row <= $#array; $row++ ) {
	  for ( my $col = 0; $col <= @{$array[$row]}; $col++ ) {
	    print "fruit";
	  }
	}
	
	$table->set_row_spacings(4);
	$table->set_col_spacings(4);
	$window->add($table);

	$window->show_all;
}

1;
