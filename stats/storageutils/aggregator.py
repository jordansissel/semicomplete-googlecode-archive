#!/usr/bin/env python

import simpledb

# rrdtoolisms
RULE_AVERAGE=1
RULE_MIN=2
RULE_MAX=3
RULE_LAST=4

class DuplicateRuleTarget(Exception):
  pass

class Rule(object):
  def __init__(self, source_key, target_key, type, xff, steps):
    self._source = source_key
    self._target = target_key
    self._type = type
    self._xff = xff
    self._steps = steps

    self._dispatch = {
      RULE_AVERAGE: lambda x: sum(x) / float(len(x)),
      RULE_MIN: lambda x: min(x),
      RULE_MAX: lambda x: max(x),
      RULE_LAST: lambda x: x[0],
    }

    self._hits = self._steps

  def Evaluate(self, unused_args, db):
    self._hits -= 1
    if self._hits > 0:
      return
    self._hits = self._steps
    values = []
    count = 0
    last_timestamp = 0
    for entry in db.ItemIteratorByRows([self._source]):
      assert entry.row == self._source
      count += 1
      values.append(entry.value)
      last_timestamp = entry.timestamp
      if count == self._steps:
        break;
    result = self._dispatch[self._type](values)
    db.Set(self._target, result, last_timestamp)

class Collector(simpledb.SimpleDB):
  def __init__(self, db_path, encode_keys=False):
    simpledb.SimpleDB.__init__(self, db_path, encode_keys=False)
    self._row_listeners = {}
    self._ruledb_path = "%s.rules" % (db_path)
    self._ruledb = simpledb.SimpleDB(self._ruledb_path, encode_keys=False)
    self._rules = {}
    self.LoadRules()

  def PurgeDatabase(self):
    simpledb.SimpleDB.PurgeDatabase(self)
    self._ruledb.PurgeDatabase()

  def LoadRules(self):
    self._ruledb.Open(create_if_necessary=True)
    for entry in self._ruledb.ItemIterator():
      self.AddRule(entry.value)

  def AddRule(self, rule):
    if rule._target in self._rules:
      raise DuplicateRuleTarget("Attempt to add rule creating rows '%s'"
                                "aborted. A rule already exists to "
                                "generate this row." % rule._target)
    self.AddRowListener(rule._source, rule.Evaluate)
    self._rules[rule._target] = rule
    self._ruledb.Set("rule", rule)

  def AddRowListener(self, row, callback, *args):
    self._row_listeners.setdefault(row, [])
    self._row_listeners[row].append((callback, args))

  def Set(self, row, value, timestamp=None):
    #print "%s @ %s" % (row, timestamp)
    simpledb.SimpleDB.Set(self, row, value, timestamp)
    if row in self._row_listeners:
      for (listener, args) in self._row_listeners[row]:
        listener(args, self)

def test():
  import random
  import time
  f = "/tmp/test2"
  db = Collector(f, encode_keys=False)
  db.PurgeDatabase()
  db.Open(create_if_necessary=True)
  start = int(time.time())

  r_hourly = Rule("hits.minute", "hits.mean.1hour", RULE_AVERAGE, 1, 60)
  r_daily = Rule("hits.minute", "hits.mean.1day", RULE_AVERAGE, 1, 60 * 24)
  db.AddRule(r_hourly)
  db.AddRule(r_daily)

  # Insert data once a minute
  for i in range(24 * 60 * 7):
    db.Set("hits.minute", random.randint(1000,2000), (start + (60 * i)) * 1000000)

  for i in db.ItemIteratorByRows(["hits.mean.1hour"]):
    print i
  for i in db.ItemIteratorByRows(["hits.mean.1day"]):
    print i

test()
