#!/usr/bin/perl

use strict;
use warnings;
use Gtk2 '-init';

require "menubar.pl";
require "toolbar.pl";
require "mac_layout.pl";

my $label1 = Gtk2::Label->new ('Label One');
my $label2 = Gtk2::Label->new ('Label Two');

my $window = Gtk2::Window->new;
my $vbox = Gtk2::VBox->new (0, 0);

my $menubar = &create_menubar;
my $toolbar = &create_toolbar;
my $layout = &create_mac_layout;
my $statusbar = Gtk2::Statusbar->new ();

$vbox->pack_start ($menubar, 0, 0, 0);
$vbox->pack_start ($toolbar, 0, 0, 0);
$vbox->add ($layout);
$vbox->pack_start ($statusbar, 0, 0, 0);
$window->add ($vbox);
$window->show_all;

Gtk2->main;
