#!/bin/sh

JOBS=${JOBS:-10}

screenlist() {
  screen -ls | awk '/^[\t ]/ {print $1}'
}

capture_all() {
  screenlist \
  | grep -v SCREENTMP \
  | xargs -n1 -P${JOBS} sh -c 'screen-session-hardcopy.sh $1 > /tmp/log.$$ 2>&1' - 
}

OUTDIR=${OUTDIR:-$(mktemp -d)}
if [ -z "$OUTDIR" -o ! -d "$OUTDIR" ] ; then
  echo "mktemp -d failed or \$OUTDIR is not a directory."
fi

export OUTDIR

capture_all

case $1 in
  -t)
    FILEPATTERN="$OUTDIR/*:p"
    shift;
    ;;
  *)
    FILEPATTERN="$OUTDIR/*"
    ;;
esac

MATCHES="$(grep -l "$@" $FILEPATTERN)"
if [ ! -z "$STY" ] ; then
  MATCHES="$(echo "$MATCHES" | grep -Fv "$STY")"
fi

for M in $MATCHES ; do
  fsty=$(echo $(basename $M) | awk -F: '{print $1}')
  fwin=$(echo $(basename $M) | awk -F: '{print $2}')
  if [ "$fwin" = "p" ] ; then
    fwin=$(grep "$@" $M | awk '{ print $1 }')
  fi

  echo "$fsty / $fwin"
done

rm -rf "$OUTDIR"
rm -f /tmp/log.*
