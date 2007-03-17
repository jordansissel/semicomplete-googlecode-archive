#!/usr/bin/perl -w

use strict;

use Gtk2 -init;
use Gtk2::Pango;

sub image_preview {
	my $file = shift;

	my ($preview_pane) = create_previewpane();

	my $vbox = Gtk2::VBox->new();
	$preview_pane->add_with_viewport($vbox);

	print STDERR "Loading Pic: " . $file->{dir} . "/" . $file->{file} . "\n";
	my $pix;
	if (defined($file->{data})) {
		my $fn = int(rand() * 100000);
		print "Data found, reading embedded image. /tmp/brows$fn\n";
		print "--> Creating fifo\n";
		open(FIFO, "> /tmp/brows$fn");
		print FIFO $file->{data};
		$pix = Gtk2::Gdk::Pixbuf->new_from_file("/tmp/brows$fn");
		close(FIFO);
		unlink("/tmp/brows$fn");
	} else {
		$pix = Gtk2::Gdk::Pixbuf->new_from_file($file->{dir} . "/" . $file->{file});
	}
        my $height = $pix->get_height;
        my $width = $pix->get_width;
        my $aspect = $width / $height;

        if ($width > 120) {
                $width = 120;
                $height = (1 / $aspect) * 120;
        }

        if ($height > 200) {
                $height = 200;
                $width = $aspect / 200;
        }

        $pix = $pix->scale_simple($width,$height,'hyper');

	my $image = Gtk2::Image->new_from_pixbuf($pix);

	$vbox->pack_start($image,0,0,0);

	my $text = create_textview();
	my $buf = $text->get_buffer();
	my $iter = $buf->get_iter_at_offset(0);
	my $typeee = $file->{type};
	$typeee =~ s!^.*/!!;
	$buf->insert($iter,"File type: " . $typeee . "\n");
	$buf->insert($iter,"Dimensions: " . $file->{img}->{x} . "x" . $file->{img}->{y});

	$vbox->pack_start($text,1,1,0);

	$vbox->show_all();
	$preview_pane->show_all();

	return $preview_pane;
}

sub generic_preview {
	print STDERR "generic_preview()\n";
	my $file = shift;

	my ($preview_pane) = create_previewpane();
	my $text = create_textview();

	my $buf = $text->get_buffer();
	my $iter = $buf->get_iter_at_offset(0);
	$buf->insert_with_tags_by_name($iter, "File: ", "title");
	$buf->insert($iter, $file->{file} . "\n\n");
	if (defined($file->{mp3})) {
		$buf->insert($iter, "MP3 File\n");
		$buf->insert_with_tags_by_name($iter, "Artist: " . $file->{mp3}->{artist} . "\n", "small");
		$buf->insert_with_tags_by_name($iter, "Song Title: " . $file->{mp3}->{song} . "\n", "small");
		$buf->insert_with_tags_by_name($iter, "Album: " . $file->{mp3}->{album} . "\n", "small");
	} else {
		$buf->insert($iter, "Contents:\n");

		if ($file->{mode} !~ m/x/ && $file->{mode} =~ m/^-/) {
		   my $woot;
		   open(FIFOFUM, "<".$file->{dir} . "/" . $file->{file});
		   read(FIFOFUM, $woot, 200);
		   close(FIFOFUM);

		   if (length($woot) == 0) {
			   $buf->insert_with_tags_by_name($iter, "<empty file>", "smallempty");
		   } else {
			   $buf->insert_with_tags_by_name($iter, $woot, "small");
		   }
		}
	}

	$preview_pane->add_with_viewport($text);

	$text->show_all();
	$preview_pane->show_all();

	return $preview_pane;
}

sub directory_preview {
	my ($dir,@contents) = @_;
	
	print STDERR "directory_preview()\n";

	my ($preview_pane) = create_previewpane();
	my $text = create_textview();
	$preview_pane->add_with_viewport($text);

	my $buf = $text->get_buffer();
	my $iter = $buf->get_iter_at_offset(0);
	$buf->insert_with_tags_by_name($iter, "Contents of:\n$dir\n-----------------\n", "title");
	$buf->insert($iter, join("\n",map("$_->{file}",@contents)));
	$buf->insert($iter, "\n");

	$text->show_all();
	$preview_pane->show_all();

	return $preview_pane;
}

sub nothing_preview {
	my ($preview_pane) = create_previewpane();
	my $text = create_textview();
	$preview_pane->add_with_viewport($text);

	my $buf = $text->get_buffer();
	my $iter = $buf->get_iter_at_offset(0);
	$buf->insert_with_tags_by_name($iter, "Preview Pane\n", "title");
	$buf->insert($iter, "This is the preview pane. When you select a file, information about that file will be displayed here. If you click an image, an thumbnail will be shown to you, etc.\n\nPlease choose a file at the left.");

	return $preview_pane;
}

sub create_textview {
	my $text = Gtk2::TextView->new();
	my $buf = $text->get_buffer();
	$text->set_editable(0);
	$text->set_cursor_visible(0);
	$text->set_wrap_mode('word');

	$buf->create_tag("title", weight=>PANGO_WEIGHT_BOLD);
	$buf->create_tag("italic", style => 'italic');
	$buf->create_tag("bold", weight => PANGO_WEIGHT_BOLD);
	$buf->create_tag("small", size => 8 * Gtk2::Pango->scale, foreground => "#666666");
	$buf->create_tag("smallempty", size => 8 * Gtk2::Pango->scale, foreground => "#ff8888");

	return $text;
}

# Not used...
sub create_actionpanel {
	my ($text,$iter) = @_;
	my $buf = $text->get_buffer(); 

	my %actions = ( Delete => \&delete_file,
			Cut    => \&cut_file,
			Copy   => \&copy_file,
			Move   => \&move_file,
			);

	while ( my ($key,$val) = each(%actions) ) {
		my $anchor = $buf->create_child_anchor($iter);
		my $event = Gtk2::EventBox->new();
		my $link = Gtk2::Label->new();
		$link->set_markup("<span foreground='blue' background='white'><u>$key</u></span>");
		$event->add($link);
		$event->signal_connect("set-focus-child" => $val);
		$link->show_all();
		$event->show_all();
		$text->add_child_at_anchor($event, $anchor);
		$buf->insert($iter,"\n");
	}
}

sub create_previewpane {
	my $preview_pane = Gtk2::ScrolledWindow->new();
	$preview_pane->set_size_request(150,300);
	$preview_pane->set_policy('automatic', 'automatic');

	return ($preview_pane);
}
1;
