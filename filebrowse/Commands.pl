#!/usr/bin/perl -w
use File::Copy;
use File::Type;
use Archive::Tar;
use File;
use strict;

sub copycmd {
  my ( $to, @files ) = @_;
  my $retval = 1;

  foreach my $file( @files ) {
    copy( $file, $to ) or $retval = 0;
  }

  return $retval;
}

sub movecmd {
  my ( $to, @files ) = @_;
  my $retval = 1;

  foreach my $file( @files ) {
    move( $file, $to ) or $retval = 0;
  }

  return $retval
}

sub mkdircmd {
  my ( $dir ) = @_;
  my $retval = 1;

  mkdir( $dir ) or $retval = 0;

  return $retval;
}

sub rmdircmd {
  my ( $dir ) = @_;
  my $retval = 1;

  rmdir( $dir ) or $retval = 0;

  return $retval;
}

sub rmcmd {
  my ( $file ) = @_;
  my $retval = 1;

  unlink( $file ) or $retval = 0;

  return $retval;
}

sub lscmd {
  my ( $path, $noMime ) = @_;

  if ( $path =~ /(\.tar\.gz|\.tar|\.tgz)$/i ) {
    $path .= "/";
  }

  if ( $path =~ /^(.*?(\.tar\.gz|\.tar|\.tgz))\/(.*?)$/i ) {
      my $tarfile = $1;
      my $newpath = $3;
      #print "\tThe Path: $tarfile $newpath\n";
      my @files;
      my @prop;
      my $tar = Archive::Tar->new();
      $tar->read( $tarfile, $tarfile =~ /\.tar\.gz|\.tgz/i );

      foreach my $file ( $tar->list_files( \@prop ) ) {
	if ( $newpath gt "" ) {
	   if ( $file =~ /^$newpath/i && $file !~ /^$newpath\/?$/ ) {
	      push( @files, new File( $file, $tar->get_content( $file ), $noMime ) );
	   }
        } elsif ( $file !~ /\// || $file =~ /\/$/ ) {
	  push( @files, new File( $file, $tar->get_content( $file ), $noMime ) );
        }
      }
      return @files;
  } else {
    my @list = glob( "$path/*" );

    for ( my $i = 0; $i <= $#list; $i++ ) {
      $list[ $i ] = new File( $list[ $i ] );
    }

    return @list;
  }
}

sub view {
  my ( $path ) = @_;

  if ( $path =~ /(.*?(\.tar\.gz|\.tar|\.tgz))\/?(.*)$/i ) {
      my $tarfile = $1;
      if ( $tarfile eq $path ) {
	$path .= "/";
      }
      my $newpath = $3;
      my @files;
      my @prop;
      my $tar = Archive::Tar->new();
      $tar->read( $tarfile, $tarfile =~ /\.tar\.gz|\.tgz/i );

      if ( $newpath gt "" && $newpath !~ /\/$/ ) {
	 return( "file", new File( $path, $tar->get_content( $newpath ) ) );
      }

      foreach my $file ( $tar->list_files( \@prop ) ) {
	if ( $newpath gt "" ) {
	   if ( $file =~ /^$newpath/i && $file !~ /^$newpath\/?$/ ) {
	      push( @files, new File( $file, $tar->get_content( $file ) ) );
	   }
        } elsif ( $file !~ /\// || $file =~ /\/$/ ) {
	  push( @files, new File( $file, $tar->get_content( $file ) ) );
        }
      }

      if ( $path =~ /\/$/ ) {
        return ( "dir", @files );
      } else {
	return ( "file", $files[ 0 ] );
      }
  }

  if ( -f $path ) {
    return ( "file", new File( $path ) );
  }

  if ( -d $path ) {
    return ( "dir", &lscmd( $path ) );
  }
}

sub chmodcmd {
  my ( $mode, $file ) = @_;
  my $retval = 1;

  chmod( $mode, $file ) or $retval = 0;

  return $retval;
}

sub execute {}

1;
