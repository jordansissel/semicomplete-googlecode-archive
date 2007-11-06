#!/bin/sh

usage() {
	echo "$0 [version]"
	exit 1;
}

if [ $# -ne 1 ]; then
  set -- $(date "+%Y%m%d")
  echo "No version specified, using $1" 
fi

TMP="/tmp"
PKG="xpathtool-$1"
DIR="${TMP}/${PKG}"
mkdir "$DIR"
cp -v xpathtool.sh LICENSE "$DIR"

tar -C /tmp -zvcf "$PKG.tar.gz" "$PKG"
rm -r $DIR
