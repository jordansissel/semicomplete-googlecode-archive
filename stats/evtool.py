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

def debug(string):
  if "DEBUG" not in os.environ:
    debug = lambda *args: True
    return

  caller = sys._getframe(1)
  code = caller.f_code
  filename = code.co_filename.split("/")[-1]
  print "%s:%d: %s" % (filename, caller.f_lineno, string)

def Update(args):
  # If 'evtool.py update foo.db -' use stdin to read updates from.
  if args[0] == "-":
    args = sys.stdin

  count = 0
  for entry in args:
    count += 1
    if count % 5000 == 0:
      debug(count)
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

def _time(func, *args):
  import time
  size = args[0]
  start = time.time()
  print "Starting %s" % func.__name__
  size = func(*args) or size
  duration = time.time() - start
  print "%s: %f/sec" % (func.__name__, (size) / duration)

def t_m1000writes(size):
  for i in range(size):
    db.Set(i - (i % 1000), i, i)

def t_reads(*args):
  count = 0
  for i in db.ItemIterator():
    count += 1
  return count

def Test(args):
  size = int(args[0])

  db.AddRule(fancydb.Rule(1000, "agg.1000", "total", 1, 10, "time"))
  db.AddRule(fancydb.Rule(2000, "agg.2000", "total", 1, 10, "time"))
  db.AddRule(fancydb.Rule(3000, "agg.3000", "total", 1, 10, "time"))

  _time(t_m1000writes, size)
  _time(t_reads, size)

def Graph(args):
  rows = args
  import datetime
  from matplotlib.dates import MonthLocator, RRuleLocator, rrulewrapper, DateFormatter, DAILY, MONDAY, MONTHLY, WeekdayLocator

  fig = figure(figsize=(5, 5))
  rc("axes", linewidth=.5, labelsize=10, titlesize=11)
  rc("xtick", labelsize=10)
  rc("ytick", labelsize=10)
  rc("font", family="monospace", size=10)
  rc("legend", fontsize=10, markerscale=0, labelsep=0)
  #rc("figure.subplot", bottom=.5)
  legend_items = []
  #count = 0
  for (index, row) in enumerate(rows):
    dates = []
    values = []
    for entry in db.ItemIteratorByRows([row]):
      (timestamp, value) = (entry.timestamp, entry.value)
      #dates.append(date2num(datetime.datetime.fromtimestamp(timestamp / 1000000)))
      dates.append(timestamp / 1000000)
      values.append(value)

    #ax = fig.add_subplot((len(rows) * 100) + 10 + count)
    #ax = fig.add_subplot(len(rows), 1, index + 1)
    ax = fig.add_subplot(1, 1, 1)
    #ax = axis()
    #count += 1
    #print dir(ax)
    #line = ax.plot_date(dates, values, '-', lw=.5)
    line = ax.plot(dates, values, '-', lw=.5)
    legend_items.append((line, "%s" % row))
    #ax.xaxis.set_major_locator(MonthLocator())
    #ax.xaxis.set_major_formatter(DateFormatter("%b '%y"))
    #rule = rrulewrapper(MONTHLY, interval=2)
    #ax.xaxis.set_major_locator(RRuleLocator(rule))
    #ax.fmt_xdata = DateFormatter('%Y-%m-%d')
    #ax.fmt_xdata = DateFormatter('%H:%M')
    ax.grid(True)
    #print sorted(dir(ax.xaxis))
    ##print sorted(dir(ax))
    #(x,y,w,h) = ax.get_position()
    #ax.set_position((x, y, w, .3))

  legend = figlegend([x[0] for x in legend_items],
           [x[1] for x in legend_items],
           'lower center')
  #fig.subplots_adjust(hspace=0.2)
  #l = legend(loc='lower center')
  #print (gca().get_position()[2]/l.get_frame().get_width(), -.3)
  #(x,y,w,h) = gca().get_position()
  #gca().set_position((x, y, w, .3))

  #print l.get_position()
  print legend.set_alpha(50)
  #print sorted(dir(legend))
  #l.get_frame().set_alpha(50)
  #rule = rrulewrapper(DAILY, interval=7)
  #ax.xaxis.set_major_locator(RRuleLocator(rule))
  #ax.xaxis.set_major_locator(WeekdayLocator(MONDAY))
  #ax.xaxis.set_major_formatter(DateFormatter("%b %d"))

  #xlabel("", font)
  #ylabel("hits/day", font)

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
    "test": Test,
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
  #print "Dying..."
