#!/usr/bin/perl -w
use MP3::Tag;

$mp3 = MP3::Tag->new($ARGV[0]);
($song, $track, $artist, $album) = $mp3->autoinfo();

print "$song :: $track :: $artist :: $album\n";
