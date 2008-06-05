#!/bin/sh

# Can't rely on $USER existing...
USER=`whoami`
UNAME=`uname`

PATTERN="$1"

case $UNAME in
  FreeBSD) psargs="-U $USER -u $USER -euww" ;;
  Linux) psargs="-U $USER -u $USER euww" ;;
  *) 
    echo "$uname is not supported"
    exit 1
    ;;
esac

ps $psargs \
| while read proc_user pid pcpu pmem vsz rss tt state start time command; do
  [ "$USER" != "$proc_user" ] && continue

  echo "$command" \
  | grep -F "$PATTERN" \
  | sed -e 's/^.* STY=\([^ ]*\).*/\1/' \
  | sort | uniq \
  | grep -vF "${STY:-NO_STY_SET}"
done
