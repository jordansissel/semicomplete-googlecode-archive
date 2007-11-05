#!/usr/bin/perl

use strict;
use warnings;
use Image::Magick;
use POSIX qw(strftime);

my $font = "/usr/lib/j2re1.5-sun/lib/fonts/LucidaSansRegular.ttf";

system("rm input_*.jpg");
foreach my $i (glob("nightmap.*.jpg")) {
  my $image = Image::Magick->new();
  my @time = split(/\./, $i);
  print "$i ", join("/", @time), "\n";
  my $text = strftime("%Y/%m/%d %H:%M GMT %z", localtime($time[1]));
  $image->Read("$i");
  $image->Annotate(font=>$font, pointsize=>24, fill=>"green", text=>$text, gravity=>"SouthEast");
  $image->Write("input_$i.jpg");
}

system("mencoder mf://input_*.jpg -mf w=602:h=250:fps=20:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi");
