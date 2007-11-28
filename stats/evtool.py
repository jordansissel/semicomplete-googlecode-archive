#!/usr/bin/env python

import sys
import os
import time
from optparse import OptionParser

from storageutils import fancydb
import numpy as N
from pylab import *

prog = sys.argv[0]
action = sys.argv[1]
file = sys.argv[2]
args = sys.argv[3:]

db = fancydb.FancyDB(file, encode_keys=False)
db.Open(create_if_necessary=True);

def Update(args):
  # If 'evtool.py update foo.db -' use stdin to read updates from.
  if args[0] == "-":
    args = sys.stdin

  count = 0
  for entry in args:
    count += 1
    if count % 5000 == 0:
      print count
    (row, value) = entry.split(":", 1)
    timestamp = long(time.time() * 1000000)
    if "@" in row:
      (row, timestamp) = row.split("@", 1)

    if value == "-":
      value = sys.stdin.read()

    for numtype in (int, float):
      try:
        value = numtype(value)
        break;
      except ValueError:
        continue

    for numtype in (int, float):
      try:
        timestamp = numtype(timestamp)
        break;
      except ValueError:
        continue

    db.Set(row, value, timestamp)

def AddRule(args):
  if len(args) != 5:
    print "Usage: source_row target_row type steps steptype"
    return 1

  (source, target, type, steps, steptype) = args
  db.AddRule(fancydb.Rule(source, target, type, 1, steps, steptype))

def Fetch(args):
  if len(args):
    iterator = db.ItemIteratorByRows(args)
  else:
    iterator = db.ItemIterator()

  for entry in iterator:
    print entry

def Create(args):
  # noop, already created
  return

def Purge(args):
  db.PurgeDatabase()

def Graph(args):
  rows = args

  from pylab import figure, show, axis, ylabel
  import datetime
  from matplotlib.dates import MonthLocator, RRuleLocator, rrulewrapper, DateFormatter, DAILY, MONDAY, WeekdayLocator

  fig = figure()
  legend_items = []
  for row in rows:
    dates = []
    values = []
    for entry in db.ItemIteratorByRows([row]):
      (timestamp, value) = (entry.timestamp, entry.value)
      dates.append(date2num(datetime.datetime.fromtimestamp(timestamp / 1000000)))
      values.append(value)

    #for (d,v) in zip(dates, values):
      #print "%s: %s" % (d, v)

    ax = fig.add_subplot(111)
    line = ax.plot_date(dates, values, '-', lw=.5)
    ax.xaxis.set_major_locator(MonthLocator())
    ax.xaxis.set_major_formatter(DateFormatter("%b '%y"))
    ax.fmt_xdata = DateFormatter('%Y-%m-%d')
    ax.grid(True)
    legend_items.append((line, "%s (per day)" % row))
    #print sorted(dir(ax))

  figlegend([x[0] for x in legend_items],
            [x[1] for x in legend_items],
            'upper right')
            
  rule = rrulewrapper(DAILY, interval=7)
  #ax.xaxis.set_major_locator(RRuleLocator(rule))
  #ax.xaxis.set_major_locator(WeekdayLocator(MONDAY))
  #ax.xaxis.set_major_formatter(DateFormatter("%b %d"))

  fig.autofmt_xdate()

  fig.savefig('hits.png', format="png")

def profile(func, *args):
  func(*args)
  return
  import hotshot
  output = "/tmp/my.profile"
  p = hotshot.Profile(output)
  p.runcall(func, *args)
  p.close()

def main(args):
  global action
  dispatch = {
    "addrule": AddRule,
    "create": Create,
    "fetch": Fetch,
    "graph": Graph,
    "update": Update,
    "purge": Purge,
  }

  if action in dispatch:
    try:
      profile(dispatch[action], args)
    except Exception, e:
      raise
    finally:
      db.Close()
  else:
    print "Unknown action '%s'" % action

if __name__ == "__main__":
  main(args)
