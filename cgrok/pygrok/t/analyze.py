#!/usr/bin/env python

import sys
import re
from pygrok import pygrok
from pygrok import patterns

PUNCT_RE = re.compile(r"""['":_.=+-?]""")

def re_weight(gre_str):
  score = 0
  score += 1.5 * (gre_str.count(" ") + gre_str.count(r"\s"))
  score += 1 + gre_str.count(".")
  score += .6 * len(PUNCT_RE.findall(gre_str))
  return score

def GetSortedPatterns():
  expansion = {}
  gre = pygrok.GrokRegex()
  gre.add_patterns(patterns.base_patterns)
  for name in patterns.base_patterns:
    gre.set_regex("%%%s%%" % name)
    expansion[name] = gre.get_expanded_regex()

  pat_list = sorted(expansion.items(), key=lambda (k,v): re_weight(v))
  pat_list.reverse()
  gre_list = []

  for (name, unused_expanded_re) in pat_list:
    gre = pygrok.GrokRegex()
    gre.add_patterns(patterns.base_patterns)
    gre.set_regex(r"%%%s=~\W%%" % (name))
    gre_list.append( (name, gre) )

  return gre_list

def RemoveUnimportantPatterns(gre_list):
  new_list = gre_list[:]
  skip_patterns = ("DATA GREEDYDATA USER USERNAME WORD NOTSPACE PID PROG "
                   "QS QUOTEDSTRING YEAR URIHOST URIPARAM URIPATH").split()
  for i in gre_list:
    if i[0] in skip_patterns:
      new_list.remove(i)
  return new_list

def Analyze(line, gre_list):
  count = -1
  while count != 0:
    count = 0
    for (name, gre) in gre_list:
      m = gre.search(line)
      if m:
        count += 1
        pos = int(m["=POSITION"])
        len = int(m["=LENGTH"])
        repl = "%%%s%%" % name
        line = line[0:pos] + repl + line[pos+len:]
  print line

def main():
  gre_list = GetSortedPatterns()
  gre_list = RemoveUnimportantPatterns(gre_list)

  for line in sys.stdin:
    Analyze(line[:-1], gre_list)

if __name__ == "__main__":
  main()
