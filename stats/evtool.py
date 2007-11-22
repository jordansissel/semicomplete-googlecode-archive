#!/usr/bin/env python

import sys
import os
import time
from optparse import OptionParser

from eventdb import RecDB
import numpy as N
from pylab import *

prog = sys.argv[0]
action = sys.argv[1]
file = sys.argv[2]
args = sys.argv[3:]

db = RecDB(file)
db.Open(create_if_necessary=True);

def Update(args):
  # If 'evtool.py update foo.db -' use stdin to read updates from.
  if args[0] == "-":
    args = sys.stdin

  for entry in args:
    (row, value) = entry.split(":", 1)
    timestamp = time.time()
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

    db.Set(row, value, timestamp)

def Fetch(args):
  if len(args):
    iterator = db.ItemIteratorByRows(args)
  else:
    iterator = db.ItemIterator()

  for entry in iterator:
    print entry

def FetchSum(args):
  time_bucket = int(args[0]) * 1000000
  aggregate_func = lambda x: N.sum(x)
  rows = args[1:]
  aggregates = db.FetchAggregate(rows, time_bucket, aggregate_func)

  from pylab import figure, show, axis
  import datetime
  from matplotlib.dates import MonthLocator, RRuleLocator, rrulewrapper, DateFormatter, DAILY, MONDAY, WeekdayLocator

  dates = []
  values = []
  for (key, value) in aggregates:
    dates.append(date2num(datetime.date.fromtimestamp(key / 1000000)))
    values.append(value)

  fig = figure()
  ax = fig.add_subplot(111)
  ax.plot_date(dates, values, '-')
  rule = rrulewrapper(DAILY, interval=7)
  ax.xaxis.set_major_locator(MonthLocator())
  ax.xaxis.set_major_formatter(DateFormatter("%B"))
  #ax.xaxis.set_major_locator(RRuleLocator(rule))
  ax.xaxis.set_minor_locator(WeekdayLocator(MONDAY))
  #ax.xaxis.set_minor_formatter(DateFormatter("%b %d"))
  #ax.autoscale_view()

  ax.fmt_xdata = DateFormatter('%Y-%m-%d')
  #ax.fmt_ydata = lambda x: "%d" % x
  ax.grid(True)
  fig.autofmt_xdate()

  fig.savefig('hits.png', format="png")
  show()



def main(args):
  global action
  dispatch = {
    "fetch": Fetch,
    "update": Update,
    "fetchsum": FetchSum,
  }

  if action in dispatch:
    dispatch[action](args)
  else:
    print "Unknown action '%s'" % action

if __name__ == "__main__":
  main(args)
