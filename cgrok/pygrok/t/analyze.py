#!/usr/bin/env python

import sys
from pygrok import pygrok
from pygrok import patterns

pattern_list = [
    "%SYSLOGBASE% Failed %WORD% for (invalid user)? %WORD:USER% from %IPORHOST% port %NUMBER%"
]

gre_list = []
for i in pattern_list:
  gre = pygrok.GrokRegex()
  gre.add_patterns(patterns.base_patterns)
  gre.set_regex(i)
  gre_list.append(gre)

for line in sys.stdin:
  for gre in gre_list:
    m = gre.search(line)
    if m:
      print m["IPORHOST"]
