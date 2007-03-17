#!/usr/bin/perl

BEGIN {
	unshift(@INC,"./gui");
	unshift(@INC,".");
}
use strict;
use warnings;
use Gtk2 '-init';

require "menubar.pl";
require "toolbar.pl";
#require "mac_layout.pl";
require "konq_layout.pl";
require "preview_pane.pl";
require "about.pl";

#close(STDOUT);
#close(STDERR);

my $window = Gtk2::Window->new;
$window->set_default_size(500,500);
my $vbox = Gtk2::VBox->new (0, 0);

my $menubar = &create_menubar;
my $toolbar = &create_toolbar;
#my $layout = &create_mac_layout;
#my $layout = $scrolledwindow;

my $layout = &create_konq_layout;

my $statusbar = Gtk2::Statusbar->new();

$vbox->pack_start ($menubar, 0, 0, 0);
$vbox->pack_start ($toolbar, 0, 0, 0);
$vbox->pack_start ($layout, 1, 1, 0);
$vbox->pack_start ($statusbar, 0, 0, 0);
$window->add ($vbox);
$window->show_all;

Gtk2->main;

sub update_statusbar
{
        my $path = shift;

        $statusbar->pop(0);
        $statusbar->push(0, $path);
}

