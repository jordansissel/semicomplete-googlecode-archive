package File;
use File::stat qw(:FIELDS);
use Stat::lsMode;
use File::Type;
use MP3::Tag;
use Image::Size;
use overload '""' => sub { $_[0]->{file} };

sub new {
  my $class = shift;

  my $sep = "/";

  my $fullPath = shift;
  if ( -d $fullPath && $fullPath !~ /$sep$/ ) {
    $fullPath .= $sep;
  }
  $fullPath =~ /(.*?)([^$sep]*)$/;

  my $data = shift;
  my $noMime = shift;

  my $dir = $1;
  my $file = $2;

  if ( $file eq "" && $dir =~ s/([^$sep]*$sep)$// ) {
    $file = $1;
  }

  my $type = '';
  my $ft = File::Type->new();

  if ( !$noMime || ! -p $fullPath ) {
  if ( $data ) {
    $type = $ft->checktype_contents( $data ) || '';
  } else {
    $type = $ft->mime_type( $fullPath ) || '';
  }
  }

  my $mp3data = undef;

  if ( $file =~ /\.mp3$/i && $data eq '' ) {
    my $mp3 = MP3::Tag->new( $fullPath );
    my ( $song, $track, $artist, $album ) = $mp3->autoinfo();
    $mp3data = {
			song => $song,
			track => $track,
			artist => $artist,
			album => $album
		   };
  }

  my $imgdata = undef;

  if ( $file =~ /(jpg|gif|jpeg|png|tiff|bmp|xpm)/i ) {
    my ( $x, $y ) = imgsize( $fullPath );
    $imgdata = { x => $x, y => $y };
  }

  if ( !$noMime || ! -p $fullPath ) {
  if ( $data ) {
    $st_size = length( $data );
    $st_atime = 0;
    $st_mtime = 0;
    $st_create = 0;
    $st_mode = 0;
  } else {
    stat( $fullPath );
    $st_mode = format_mode( $st_mode );
  }
  }

  my $hsize = $st_size;
  if ( $hsize < 1000 ) {
    $hsize = sprintf( "%d bytes", $hsize );
  } elsif ( $hsize < 1000000 ) {
    $hsize /= 1000.0;
    $hsize = sprintf( "%.2f kB", $hsize );
  } elsif ( $hsize < 1000000000 ) {
    $hsize /= 1000000.0;
    $hsize = sprintf( "%.2f MB", $hsize );
  } elsif ( $hsize < 1000000000.0 ) {
    $hsize /= 1000000000.0;
    $hsize = sprintf( "%.2f GB", $hsize );
  }

  my $info = { 
		full => $fullPath,
		file => $file, 
		dir => $dir,
		size => $st_size,
		hsize => $hsize,
		access => $st_atime,
		modify => $st_mtime,
		create => $st_create,
		mode => $st_mode,
		type => $type,
		mp3 => $mp3data,
		img => $imgdata,
		data => $data
   	     };

  bless $info, $class;
  return $info;
}
1;
