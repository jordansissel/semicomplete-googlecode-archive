use strict;
use Gtk2;

BEGIN {
#unshift(@INC,'/home/group/s99/redbull0/code');
	unshift(@INC,'../');
}

use File;

require "preview_pane.pl";
require "Commands.pl";

my ($k_viewport, $k_model, $k_hbox, $k_tree, $k_prev, $k_pwd, $k_box, $k_sel, $k_tmp);

my $C = {
		FILECOLUMN => 0,		# File name
		DATECOLUMN => 1,		# Date Modified
		SIZECOLUMN => 2,		# Size
		PERMCOLUMN => 3,		# Permissions
	};


sub create_konq_layout {
	my $pwd = $ENV{PWD};
	my $hbox = Gtk2::HBox->new();
	my $viewport = Gtk2::ScrolledWindow->new();
	$viewport->set_policy('automatic', 'automatic');

	my $prev = nothing_preview();
	$k_prev = $prev;

	my $model = Gtk2::ListStore->new('Glib::String', 
					 'Glib::String',
					 'Glib::String',
					 'Glib::String',
					 'Glib::String');
	my $tree = Gtk2::TreeView->new_with_model($model);

	$k_viewport = $viewport;
	$k_model = $model;
	$k_tree = $tree;
	$k_pwd = $pwd;
	$k_hbox = $hbox;

	#foreach my $d ("hello", "there", "how", "are", "you") {
		#my $iter = $model->append();
		#$model->set($iter, 0, $d);
	#}

	populate_list($pwd);

	my $rend = Gtk2::CellRendererText->new();

	my $column = Gtk2::TreeViewColumn->new_with_attributes("Filename", $rend, 'text', 0);
	$tree->append_column($column);

	my $column3 = Gtk2::TreeViewColumn->new_with_attributes("Size", $rend, 'text', 2);
	my $column4 = Gtk2::TreeViewColumn->new_with_attributes("Mode", $rend, 'text', 3);
	$tree->append_column($column3);
	$tree->append_column($column4);

	$tree->signal_connect(row_activated => \&file_double_click, $model);

	$tree->expand_all();
	$tree->show_all();

	my $selection = $tree->get_selection();
	#$selection->set_mode('browse');
	$selection->signal_connect(changed => \&row_clicked, $model);

	$viewport->add_with_viewport($tree);

	$tree->show_all();
	$viewport->show_all();

	$hbox->pack_start($viewport,1,1,0);

	$k_box = Gtk2::VBox->new();
	$k_box->pack_start($prev, 0, 0, 0);
	#$hbox->pack_start($prev,1,1,0);
	$k_box->show_all();
	$hbox->show_all();

	return $hbox;
}

sub row_clicked {
	# We clicked a file, generate a preview.
	my ($selection, $model) = @_;

	my $iter = $selection->get_selected();
	return unless defined $iter;

	my ($name) = $model->get($iter, 0);
	print "File: $name\n" if ($name);
	$k_sel = $name;
	update_statusbar("$k_pwd/$k_sel");
	print "Dir: $k_pwd\n";

	print "Calling view($k_pwd/$name)\n";
	my ($type, @objs) = view($k_pwd . "/" . $name);

	print "Type: $type\n";
	print "Objects: " . scalar(@objs) . "\n"; 

	if ($type eq 'file') {
		# Pull magical data hottness
		my $info = $objs[0];
		print "Mime type: " . $info->{'type'} . "\n";

		if ($info->{'type'} =~ m!^image/!) {
			$k_prev->destroy() if ($k_prev);
			$k_prev = undef;
			$k_prev = image_preview($info);
			$k_hbox->pack_start($k_prev, 0, 0, 0);
		} else {
			$k_prev->destroy() if ($k_prev);
			$k_prev = undef;
			$k_prev = generic_preview($info);
			$k_hbox->pack_start($k_prev, 0, 0, 0);
		}
	} elsif ($type eq 'dir') {
		$k_prev->destroy() if ($k_prev);
		$k_prev = undef;
		$k_prev = directory_preview($name, @objs);
		$k_hbox->pack_start($k_prev, 0, 0, 0);
	}
}

sub file_double_click {
	# We double clicked a file.
	my ($tree, $path, $column) = @_;
	
	my $model = $tree->get_model();

	my $iter = $model->get_iter($path);
	my ($file) = $model->get($iter);

	my ($type, @objs) = view($k_pwd . "/" . $file);

	print "Double click on $k_pwd/$file\n";

	if ($type eq 'dir' || $file eq '..') {
		$k_pwd .= "/" . $file;
		$k_pwd =~ s!//+!/!g;
		$k_pwd =~ s!/[^/]*/..$!!;
		print "Repopulating directory with $k_pwd as base\n";
		populate_list($k_pwd);
	}
}

sub populate_list {
	my ($dir) = @_;
	print "populate_list($dir)\n";

	$k_model->clear();

	print "Dir: $dir\n";
	my @files = lscmd($dir);

	my $tmp = $dir;
	$tmp =~ s!/[^/]+/?$!!;
	my @parent = view($tmp);
	print "Checking parent: $tmp\n";
	unshift(@files,$parent[1]); # Parent
	$files[0]->{file} = "..";

	foreach my $d (@files) {
		my $iter = $k_model->append();
		#print "File: " . $C->{FILECOLUMN} . $d->{file} . "\n";
		#print "Size: " . $C->{SIZECOLUMN} . $d->{hsize} . "\n";
		#print "Date: " . $d->{modify} . "\n";

		$k_model->set($iter, $C->{FILECOLUMN}, $d->{file},
				     $C->{SIZECOLUMN}, $d->{hsize},
				     $C->{DATECOLUMN}, $d->{modify},
				     $C->{PERMCOLUMN}, $d->{mode},
				     );
	}
}

sub back_cb {
	print STDERR "back_cb()\n";
}

sub fwd_cb {
	print STDERR "fwd_cb()\n";
}

sub up_cb {
	print STDERR "up_cb()\n";
	$k_pwd =~ s!/[^/]+/?$!/! unless ($k_pwd eq '/');
	populate_list($k_pwd);
	update_statusbar("$k_pwd");
}

sub cut_cb {
	print STDERR "cut_cb()\n";
	# Cut
	my $fn = int(rand * 10000);
	$k_tmp = "/tmp/cutfile".$fn."_".$k_sel;
	print "Cutting $k_sel to $k_tmp\n";
	movecmd($k_tmp,"$k_pwd/$k_sel");
	populate_list($k_pwd);
}

sub copy_cb {
	print STDERR "copy_cb()\n";
	my $fn = int(rand * 10000);
	$k_tmp = "/tmp/cutfile".$fn."_".$k_sel;
	print "Copying $k_sel to $k_tmp\n";
	copycmd("$k_tmp", "$k_pwd/$k_sel");
}

sub paste_cb {
	print STDERR "paste_cb()\n";
	my $tmp = $k_tmp;
	$tmp =~ s!^/tmp/cutfile[0-9]+_!!;
	print "Moving $k_tmp to $k_pwd/$tmp\n";
	movecmd("$k_pwd/$tmp","$k_tmp");
	undef($k_tmp);
	populate_list($k_pwd);
}

sub delete_cb {
	print STDERR "delete_cb()\n";
	rmcmd("$k_pwd/$k_sel");
	populate_list($k_pwd);
}

1;
