#!/usr/bin/env python

import sys
import re
from pygrok import pygrok
from pygrok import patterns

gre = pygrok.GrokRegex()
gre.add_patterns(patterns.base_patterns)
gre.set_regex(sys.argv[1])
#print gre.get_expanded_regex()
for i in sys.stdin:
  m = gre.search(i[:-1])
  if m:
    print m
