use strict;

BEGIN {
	unshift(@INC,'/home/group/s99/redbull0/code');
}

require "Commands.pl";

my %mount_points = ("Home" => "$ENV{'HOME'}",
			"/" => "/");

sub create_mount_pane ()
{
	#
	my $sw = Gtk2::ScrolledWindow->new ();
	my $model = Gtk2::ListStore->new ('Glib::String');
	my $tree = Gtk2::TreeView->new_with_model ($model);

	#
	foreach my $entry (keys %mount_points) {
		my $iter = $model->append ();
		$model->set ($iter, 0, $entry);
	}

	#
	my $rend_text = Gtk2::CellRendererText->new ();
	my $column = Gtk2::TreeViewColumn->new_with_attributes ("", $rend_text, 'text', 0);

	#
	my $selection = $tree->get_selection();
	$selection->signal_connect (changed => \&left_row_clicked, $model);

	#
	$tree->append_column ($column);
	$tree->set_headers_visible (0);
	$sw->add_with_viewport($tree);

	return $sw;
}

sub create_tree_pane ()
{
}

sub create_model ()
{
	my @entries = @_;

	my $sw = Gtk2::ScrolledWindow->new();
	$sw->set_policy('never', 'automatic');

	my $model = Gtk2::ListStore->new('Glib::String');
	my $treeview = Gtk2::TreeView->new_with_model($model);

	for (my $i = $@entries - 1, $i <= 0, $i--) {
		my $iter = $model->append
	}
}
		

sub create_mac_layout ()
{
	my $hpane = Gtk2::HPaned->new ();
	
	$hpane->add1 (&create_mount_pane ());
	$hpane->add2 (Gtk2::Viewport->new ());

	return $hpane;
}

1;
