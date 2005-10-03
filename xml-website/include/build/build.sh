#!/bin/ksh

WORKDIR=.generate
TOOLSDIR=/u9/psionic/public_html/new/build
SPWD=$PWD
DEPTH=${DEPTH:-0}
CWD=

#INDENT='sed -e "s/^/   [$$] /"'
INDENT='sed -e "s/^/   /"'

debug() {
	echo $* 1>&2
}

mktemp() {
	#RAND=`perl -e 'read(STDIN,$foo,4); print unpack("%I",$foo)' 2>/dev/null < /dev/random`

	echo .xmlbuild.$$.${RANDOM}.$1
}

mkworkdir() {
	[ $1 = "." ] && set ""
	#echo "MKDIR: $1"
	if [ $# -gt 0 ]; then
		[ -d "$WORKDIR/$1" ] || mkdir -p $WORKDIR/$1
	else
		[ -d "$WORKDIR" ] || mkdir $WORKDIR
	fi
}

_xmllint() {
	debug "= xmllint: $1"
	xmllint --xinclude $1
}

_xsltproc() {
	SAFEFILE=`echo "$1" | sed -e 's,[ /],_,g'`
	TMPLINT=${WORKDIR}/$CWD/`mktemp $SAFEFILE.xmllint`
	TMPOUT=${WORKDIR}/$CWD/`mktemp $SAFEFILE.output`
	_xmllint $1 > $TMPLINT

	NEEDXSL=`xsltproc ${TOOLSDIR}/need_xslt.xsl $1`
	if [ "$NEEDXSL" = "YES" ]; then
		debug "= xsltproc: $1"
		echo xsltproc --path $WORKDIR/$CWD --xinclude $TMPLINT 1>&2
		xsltproc --path $PWD --xinclude $TMPLINT > $TMPOUT
	else 
		cp $TMPLINT $TMPOUT
	fi
	cat $TMPOUT
	rm $TMPOUT $TMPLINT
}

process_file() {
	OPWD=`pwd`
	FILE=$1
	INCLUDE=`echo "$FILE" | grep '.xmli$'`

	DIR=`dirname $FILE`
	FILE=`basename $FILE`
	if [ ! -z "$INCLUDE" ]; then
		FILE=${FILE%.xmli}.xml
		INCLUDE=1
	fi

	SAFEFILE=`echo "$DIR/$FILE" | sed -e 's,[ /],_,g'`
	mkworkdir $DIR
	CWD=$DIR

	# Check dependencies
	${TOOLSDIR}/xmlmkdep $DIR/$FILE | sh -c "$INDENT"

	# Process dependencies
	for i in `cat .depends.$SAFEFILE`; do
		DEPTH=$(($DEPTH + 1)) $0 $DIR/$i 2>&1 | sh -c "$INDENT"
	done

	if [ ! -f "$WORKDIR/$DIR/$FILE" -o "$FILE" -nt "$WORKDIR/$DIR/$FILE" ]; then
		echo "= Processing $FILE"
		# Process myself now
		_xsltproc $DIR/$FILE > $WORKDIR/$DIR/$FILE

		if [ "$DEPTH" -eq 0 ]; then
			set -x
			cp $WORKDIR/$DIR/$FILE $DIR/${FILE%.xml}.html
			set +x
		fi
	else
		echo "= $FILE up to date"
	fi
}


# Go!

if [ $# -eq 0 ]; then
	for i in *.xml; do 
		process_file $i
	done
else
	while [ $# -gt 0 ]; do
		process_file $1
		shift
	done
fi

