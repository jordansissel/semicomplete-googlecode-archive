#!/bin/sh

JOBS=${JOBS:-10}

screenlist() {
  screen -ls | awk '/^[\t ]/ {print $1}'
}

capture_all() {
  screenlist \
  | egrep -v SCREENTMP \
  | xargs -n1 -P${JOBS} sh -c 'screen-session-hardcopy.sh $1 > /tmp/log.$$ 2>&1' - 
}

OUTDIR=${OUTDIR:-$(mktemp -d)}
if [ -z "$OUTDIR" -o ! -d "$OUTDIR" ] ; then
  echo "mktemp -d failed or \$OUTDIR is not a directory."
fi

export OUTDIR

capture_all

FILES="$(ls $OUTDIR)"

set -- `getopt twl "$@"`
while [ $# -gt 0 ]; do
  case $1 in
    -t)
      FILES="$(ls $OUTDIR/*:p)"
      ;;
    -w)
      FILES="$(ls $OUTDIR/* | grep -v ':p$')"
      ;;
    -l)
      LOCATIONONLY=1
      FILES="$(ls $OUTDIR/*:p)"
      ;;
    --) 
      break
      ;;
  esac
  shift;
done

MATCHES="$(grep -l "$@" $FILES)"

# Filter out our current screen session if we're running this from
# within a screen already.
if [ ! -z "$STY" ] ; then
  MATCHES="$(echo "$MATCHES" | grep -Fv "$STY")"
fi

for M in $MATCHES ; do
  fsty=$(echo $(basename $M) | awk -F: '{print $1}')
  fwin=$(echo $(basename $M) | awk -F: '{print $2}')
  if [ "$fwin" = "p" ] ; then
    if [ "$LOCATIONONLY" -eq 1 ] ; then
      linecmp=$(awk 'NF > 1 {print $(NF - 1)}' $M \
                | egrep -n "$@" \
                | awk -F: '{printf("NR == %d ||", $1)}' \
                | sed -e 's/ ||$//')
      fwin="$(awk "$linecmp {print \$1}" $M)"
    else
      fwin="$(egrep "$@" $M | awk '{ print $1 }')"
    fi
  fi

  echo "$fsty / $fwin"
done

rm -rf "$OUTDIR"
rm -f /tmp/log.*
