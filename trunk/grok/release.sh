#!/bin/sh

usage() {
	echo "$0 [version]"
	exit 1;
}

[ $# -ne 1 ] && usage

TMP="/tmp"
GROK="grok-$1"
DIR="${TMP}/${GROK}"
mkdir "$DIR"
cp -v grok grok.1 grok.conf "$DIR"
tar -C /tmp -vcf grok-$1.tar.gz "$GROK"
rm -r $DIR
