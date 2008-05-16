#!/usr/bin/perl

use strict;
use warnings;
use Image::Magick;
use POSIX qw(strftime);

my $font = "/usr/lib/j2re1.5-sun/lib/fonts/LucidaSansRegular.ttf";

my $tmp = $ARGV[0];
shift(@ARGV);
my @idx = @ARGV;

sub blend {
  my ($idx, $image, @images) = @_;
  my $blends = 2;
  my $blend_image = Image::Magick->new();
  $blend_image->Read($images[$idx]);
  $blend_image->GaussianBlur(geometry=>5);
  $blends--;
  while ($blends > 0) {
    my $i = $idx - $blends;
    $blends--;
    next if ($i < 0);

    my $cur_image = Image::Magick->new();
    $cur_image->Read($images[$idx]);
    $blend_image->Composite($cur_image, compose=>"blend", blend=>"50%");
    $blend_image->MotionBlur(radius=>50, angle=>1);
  }
  return $blend_image
}

my @images = glob("mapdata/nightmap.*.jpg");

for my $idx (@idx) {
  my $i = $images[$idx];
  my @f = split("/", $i);
  my $f = $f[1];
  my $output = "$tmp/input_$f";
  my @ostat = stat($output); my $omtime = $ostat[9] || 0;
  my @istat = stat($i); my $imtime = $istat[9] || 0;
  exit if ($omtime > $imtime);
  my $image = Image::Magick->new();

  my @time = split(/\./, $f);
  $image->Read("$i");
  exit if ($image->Get("width") != 602);
  print STDERR "$f ", join("/", @time), " / $idx\n";

  my $text = strftime("%Y/%m/%d %H:%M GMT %z", localtime($time[1]));
  my $blend_image = blend($idx, $image, @images);
  $image->Composite($blend_image, compose=>"blend", blend=>"50%");
  $image->Annotate(font=>$font, pointsize=>24, fill=>"green", text=>$text, gravity=>"SouthEast");

  $text = strftime("%A", localtime($time[1]));
  $image->Annotate(font=>$font, pointsize=>20, fill=>"green", text=>$text, gravity=>"NorthEast");

  $image->Write($output);
}
