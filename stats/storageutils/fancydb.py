#!/usr/bin/env python

import simpledb
import time
import threading

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
    for name in funcs:
      if name in ("lock", "release"):
        continue
      if hasattr(getattr(self, name), "__call__"):
        setattr(self, name, synchronized(getattr(self, name)))

  def lock(self):
    self.__lock.acquire()

  def release(self):
    self.__lock.release()

class RuleEvaluator(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)
    self._notifications = ThreadSafeDict()
    self._done= threading.Event()
    self._done.clear()

  def run(self):
    done = False
    while not done:
      done = self._done.isSet()
      print "Eval iteration"
      self._notifications.lock()
      if len(self._notifications.keys()):
        self.ProcessEvaluations()
      self._notifications.release()

      # sleep for a little
      # XXX: If we try to evaluate while updating, segfault happens.
      self._done.wait(1)

  def Notify(self, notification):
    self._notifications.setdefault(notification, 0)
    self._notifications[notification] += 1

  def ProcessEvaluations(self):
    keys_to_remove = []
    for notification in self._notifications:
      rule = notification._rule
      rule.Evaluate(notification._start)
    for notification in keys_to_remove:
      self._notifications[notification]

  def Finish(self):
    self._done.set()

class RuleNotification(object):
  def __init__(self, rule, start, end):
    self._rule = rule
    self._start = start
    self._end = end

  def __hash__(self):
    return hash(self._rule) + hash(self._start) + hash(self._end)

  def __cmp__(self, other):
    return cmp(self._rule, other._rule) \
           or cmp(self._start, other._start) \
           or cmp(self._end, other._end)

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
    self._db = None

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

  def SetDB(self, db):
    self._db = db

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

  def Notify(self, unused_args, db, timestamp):
    # Record a notification somewhere
    if self._step_type == STEP_COUNT:
      raise NotImplemented("COUNT not implemented for rules")
    if not self._evaluator:
      raise NotImplemented("No evaluator for this rule. Cannot notify.")
    (start, end) = self.GetTimeRangeForTimestamp(timestamp)
    #print "schedule eval for: (%s) %s, %s" % (timestamp/1000000, start/1000000, end/1000000)

    notification = RuleNotification(self, start, end)
    self._evaluator.Notify(notification)

  def GetTimeRangeForTimestamp(self, timestamp):
    period = self._steps * 1000000
    start = timestamp - (timestamp % period)
    end = start + period
    return (start, end)

  def Evaluate(self, timestamp, *args):
    print "Evaluate called"
    if self._step_type == STEP_TIME:
      self.EvaluateTime(timestamp, *args)
    elif self._step_type == STEP_COUNT:
      self.EvaluateCount(timestamp, *args)

  def EvaluateCount(self, timestamp, *args):
    assert(False)
    self._hits += 1
    if self._hits < self._steps:
      return
    self._hits = 0
    count = 0
    values = []
    last_timestamp = None
    for entry in self._db.ItemIteratorByRows([self._source]):
      assert entry.row == self._source
      count += 1
      values.append(entry.value)
      last_timestamp = entry.timestamp
      if count == self._steps:
        break;

  def EvaluateTime(self, timestamp, *args):
    #print "time eval"
    #for i in self._db.ItemIterator():
      #print "%s: %s" % (timestamp, i)
    (start, end) = self.GetTimeRangeForTimestamp(timestamp)
    #print "range: (%s) (%s, %s)" % (timestamp/1000000, end/1000000, start/1000000)
    values = []
    last_timestamp = None
    count = 0
    for entry in self._db.ItemIteratorByRows([self._source]):
      count += 1
      #print "evaltime: %d" % (entry.timestamp/1000000)
      if entry.timestamp > end:
        #print "cont: %s > (%s, %s)" % (entry.timestamp/1000000, end/1000000, start/1000000)
        continue
      if entry.timestamp < start:
        #print "break: %s > (%s, %s)" % (entry.timestamp/1000000, end/1000000, start/1000000)
        break
      #print "found: %s" % (entry.timestamp / 1000000)
      values.append(entry.value)
      last_timestamp = entry.timestamp
    print "Count: %d" % count

    if last_timestamp is not None:
      result = self._dispatch[self._ruletype](values)
      print "Set: %s@%s => %s" % (self._target, end, result)
      self._db.Set(self._target, result, end)

class FancyDB(simpledb.SimpleDB):
  def __init__(self, db_path, encode_keys=False):
    simpledb.SimpleDB.__init__(self, db_path, encode_keys=False)
    self._row_listeners = {}
    self._ruledb = simpledb.SimpleDB(db_path, db_name="rules", encode_keys=False)
    self._rules = {}
    self._evaluator = RuleEvaluator()
    self._evaluator.start()

  def Open(self, create_if_necessary=True):
    simpledb.SimpleDB.Open(self, create_if_necessary)
    self._ruledb.Open(create_if_necessary=True)
    self.LoadRules()

  def Close(self):
    self._evaluator.Finish()
    self._evaluator.join()
    simpledb.SimpleDB.Close(self)
    self._ruledb.Close()

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
    rule.SetDB(self)
    self.AddRowListener(rule._source, rule.Notify)
    self._rules[rule._target] = rule
    if save_rule:
      self._ruledb.Set("rule", rule.ToDict())

  def AddRowListener(self, row, callback, *args):
    self._row_listeners.setdefault(row, [])
    self._row_listeners[row].append((callback, args))

  def Set(self, row, value, timestamp=None):
    #print "%s @ %s" % (row, timestamp)
    simpledb.SimpleDB.Set(self, row, value, timestamp)
    if row in self._row_listeners:
      for (listener, args) in self._row_listeners[row]:
        #listener(args, self, row, value, timestamp)
        listener(args, self, timestamp)

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
