#!/bin/sh

query="$1"

if [ -z "$query" ] ; then
  echo "No query given"
  exit 1
fi

jquery="table.ddrsteps"

./jquerycmd.sh --url "http://maps.google.com/maps?f=q&hl=en&q=${query}&ie=UTF8&z=10&om=1" --query "$jquery" \
| sed -e 's,<[^>]*class="[^"]*dirsegnote[^"]*"[^>]*>[^<]*</[^>]*>,,g' \
| xpathtool.sh --ihtml '//td[@class="dirsegtext"]|//div[@id="sxdist"]' 2> /dev/null \
| paste - -  \
| col
