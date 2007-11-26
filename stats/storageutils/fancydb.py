#!/usr/bin/env python

import simpledb
import time

# rrdtoolisms
RULE_AVERAGE=1
RULE_MIN=2
RULE_MAX=3
RULE_LAST=4
RULE_TOTAL=5

STEP_COUNT=1
STEP_TIME=2

OP_GET=1
OP_SET=2
OP_DELETE=3

class DuplicateRuleTarget(Exception):
  pass

def StringToType(string):
  return {
    "average": RULE_AVERAGE,
    "min": RULE_MIN,
    "max": RULE_MAX,
    "last": RULE_LAST,
    "total": RULE_TOTAL,
    "time": STEP_TIME,
    "count": STEP_COUNT,
  }[string]

def synchronized(func):
  def newfunc(self, *args, **kwds):
    assert hasattr(self, "__lock")
    self.__lock.acquire()
    func(self, *args, **kwds)
    self.__lock.release()
  return newfunc

class ThreadSafeDict(dict):
  def __init__(self, *args, **kwds):
    dict.__init__(self, *args, **kwds)
    self.__lock = threading.RLock()
    funcs = self.__dict__.keys()
    for name in funcs
      setattr(self, name) = synchronized(getattr(self, name))

  def lock(self):
    self.__lock.acquire()

  def release(self):
    self.__lock.release()

class RuleEvaluator(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self._notifications = ThreadSafeDict()

  def run(self):
    while True:
      print "Eval iteration"
      self._notifications.lock()
      if len(self._notifications.keys()):
        self.ProcessEvaluations()
      self._notifications.release()
      time.sleep(10)

  def Notify(self, notification):
    self._notifications.setdefault(notification, 0)
    self._notifications[notification] += 1

  def ProcessEvaluations(self):
    for i in self._notifications:
      print "%s: %s" % (i, self._notifications[i])
      del self._notifications[i]

class RuleNotification(object):
  def __init__(self, rule, start, end):
    self._rule = rule
    self._start = start
    self._end = end

  def __hash__(self):
    return hash(self._rule) + hash(self._start) + hash(self._end)

class Rule(object):
  init_attrs = ("_source", "_target", "_ruletype", "_xff", "_steps", "_step_type")
  restore_attrs = ("_hits",)

  def __init__(self, source, target, ruletype, xff, steps, step_type):
    self._source = source
    self._target = target
    self._ruletype = ruletype
    self._xff = int(xff)
    self._steps = int(steps)
    self._step_type = step_type

    # Rule Evaluator
    self._evaluator = None

    if isinstance(self._ruletype, str):
      self._ruletype = StringToType(self._ruletype)
    if isinstance(self._step_type, str):
      self._step_type = StringToType(self._step_type)

    if self._step_type == STEP_TIME:
      self._next_timestamp = 0

    self._dispatch = {
      RULE_AVERAGE: lambda x: sum(x) / float(len(x)),
      RULE_MIN: lambda x: min(x),
      RULE_MAX: lambda x: max(x),
      RULE_LAST: lambda x: x[0],
      RULE_TOTAL: lambda x: sum(x),
    }

    self._hits = 0

  def __hash__(self):
    return hash(self._target)

  def SetEvaluator(self, evaluator):
    self._evaluator = evaluator

  def ToDict(self):
    attrs = self.init_attrs + self.restore_attrs
    data = {}
    for i in attrs:
      data[i] = getattr(self, i)
    return data

  @classmethod
  def CreateFromDict(self, data):
    """ Factory-type method to create a Rule instance from a dictionary.

    For a Rule instance 'obj', 
      Rule.CreateFromDict(obj.ToDict()) == obj
    """
    args = {}
    for i in self.init_attrs:
      args[i[1:]] = data[i]
    obj = Rule(**args)
    for i in self.restore_attrs:
      setattr(obj, i, data[i])
    return obj

  def Notify(self, unused_args, db, row, value, timestamp):
    # Record a notification somewhere
    if self._step_type == STEP_COUNT:
      raise NotImplemented("COUNT not implemented for rules")
    if not self._evaluator
      raise NotImplemented("No evaluator for this rule. Cannot notify.")
    (start, end) = self.GetTimeRangeForTimestamp(timestamp)

    key = (self._target, start, end)
    self._evaluator.

  def GetTimeRangeForTimestamp(self, timestamp)
    period = self._steps * 1000000
    end = timestamp - (timestamp % period)
    start = end - period
    return (start, end)

  def Evaluate(self, unused_args, db, row, value, timestamp):
    if self._step_type == STEP_TIME:
      self.EvaluateTime(unused_args, db, row, value, timestamp)
    elif self._step_type == STEP_COUNT:
      self.EvaluateCount(unused_args, db, row, value, timestamp)

  def EvaluateCount(self, unused_args, db, row, value, timestamp):
    self._hits += 1
    if self._hits < self._steps:
      return
    self._hits = 0
    count = 0
    values = []
    last_timestamp = None
    for entry in db.ItemIteratorByRows([self._source]):
      assert entry.row == self._source
      count += 1
      values.append(entry.value)
      last_timestamp = entry.timestamp
      if count == self._steps:
        break;

  def EvaluateTime(self, unused_args, db, row, value, timestamp):
    if timestamp < self._next_timestamp:
      return
    #print "time eval"
    (start, end) = self.GetTimeRangeForTimestamp(timestamp)
    period = self._steps * 1000000
    end = timestamp - (timestamp % period)
    start = bucket - period
    end = bucket
    values = []
    last_timestamp = None
    for entry in db.ItemIteratorByRows([self._source]):
      if entry.timestamp > end:
        continue
      if entry.timestamp < start:
        break;
      values.append(entry.value)
      last_timestamp = entry.timestamp

    print "val: %d" % len(values)
    print "(%d, %d)" % (start / 1000000, end / 1000000)

    self._next_timestamp = end
    if last_timestamp is not None:
      result = self._dispatch[self._ruletype](values)
      db.Set(self._target, result, last_timestamp)

class FancyDB(simpledb.SimpleDB):
  def __init__(self, db_path, encode_keys=False):
    simpledb.SimpleDB.__init__(self, db_path, encode_keys=False)
    self._row_listeners = {}
    self._ruledb = simpledb.SimpleDB(db_path, db_name="rules", encode_keys=False)
    self._rules = {}
    self._evaluator = RuleEvaluator()

  def Open(self, create_if_necessary=True):
    simpledb.SimpleDB.Open(self, create_if_necessary)
    self._ruledb.Open(create_if_necessary=True)
    print self._dbenv
    self.LoadRules()

  def PurgeDatabase(self):
    simpledb.SimpleDB.PurgeDatabase(self)
    self._ruledb.PurgeDatabase()

  def LoadRules(self):
    for entry in self._ruledb.ItemIterator():
      self.AddRule(Rule.CreateFromDict(entry.value), save_rule=False)

  def AddRule(self, rule, save_rule=True):
    if rule._target in self._rules:
      raise DuplicateRuleTarget("Attempt to add rule creating rows '%s'"
                                "aborted. A rule already exists to "
                                "generate this row." % rule._target)
    rule.SetEvaluator(self._evaluator)
    self.AddRowListener(rule._source, rule.Notify)
    self._rules[rule._target] = rule
    if save_rule:
      self._ruledb.Set("rule", rule.Pickleable())

  def AddRowListener(self, row, operation, callback)
    self._row_listeners.setdefault(row, [])
    self._row_listeners[row].append((callback, args))

  def Set(self, row, value, timestamp=None):
    #print "%s @ %s" % (row, timestamp)
    simpledb.SimpleDB.Set(self, row, value, timestamp)
    if row in self._row_listeners:
      for (listener, args) in self._row_listeners[row]:
        listener(args, self, row, value, timestamp)

def test():
  import random
  import time
  db = FancyDB(f, encode_keys=False)
  db.Open(create_if_necessary=True)
  return
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

#test()
