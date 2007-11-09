#!/usr/bin/perl

use strict;
use warnings;
use Image::Magick;
use POSIX qw(strftime);

my $font = "/usr/lib/j2re1.5-sun/lib/fonts/LucidaSansRegular.ttf";

my $tmp = $ARGV[0];

foreach my $i (glob("mapdata/nightmap.*.jpg")) {
  my @f = split("/", $i);
  my $f = $f[1];
  my $output = "$tmp/input_$f";
  my @ostat = stat($output); my $omtime = $ostat[9] || 0;
  my @istat = stat($i); my $imtime = $istat[9] || 0;
  next if ($omtime > $imtime);
  my $image = Image::Magick->new();
  my @time = split(/\./, $f);
  print STDERR "$f ", join("/", @time), "\n";
  $image->Read("$i");
  next if ($image->Get("width") != 602);

  my $text = strftime("%Y/%m/%d %H:%M GMT %z", localtime($time[1]));
  $image->Annotate(font=>$font, pointsize=>24, fill=>"green", text=>$text, gravity=>"SouthEast");

  $text = strftime("%A", localtime($time[1]));
  $image->Annotate(font=>$font, pointsize=>20, fill=>"green", text=>$text, gravity=>"NorthEast");

  $image->Write($output);
}

#system("mencoder mf://input_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi");
