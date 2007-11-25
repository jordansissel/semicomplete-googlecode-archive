#!/usr/bin/env python

from bsddb import db

import os
import time
import struct
import cPickle

# database:
# timestamp should be 64bit value: epoch in milliseconds == 52 bits.
# row should be 64bit id
# key = row + timestamp

# if row -> id not exists, add to database

BDB_ACCESS_FLAGS = db.DB_BTREE
NEXT_ID_KEY = "_next_id"

class DBExists(Exception):
  pass

class RowNotFound(Exception):
  pass

class DBNotOpened(Exception):
  pass

# use big endian packing since little endian sucks at sorting.
def To64(num):
  return struct.pack(">Q", long(num))

def From64(data):
  return struct.unpack(">Q", data)[0]

def KeyToRowAndTimestamp(key):
  return (key[:-8], From64(key[-8:]))

def EndRow(row):
  x = ord(row[-1])
  if x == 255:
    return EndRow(row[:-1])
  return row[:-1] + chr(x + 1)

class Entry(object):
  def __init__(self, row, timestamp, value):
    self.row = row
    self.timestamp = timestamp
    self.value = value

  def __str__(self):
    return "%s[%s]: %s" % (self.row, self.timestamp, self.value)

  def __repr__(self):
    return str(self)

class SimpleDB(object):
  """ Simple timestamped key-value pair storage space. 

  Backed with BDB.
  """

  def __init__(self, db_path, encode_keys=False):
    self._db_path = db_path
    self._dbh = None

    self._dbe = db.DBEnv()
    self.use_key_db = encode_keys
    if self.use_key_db:
      self._keydb_path = "%s.keys" % self._db_path
      self._keydb = None

  def Open(self, create_if_necessary=False):
    if create_if_necessary and not os.path.exists(self._db_path):
      self._CreateBDB()

    self._dbh = db.DB()
    self._OpenBDB()

    if self.use_key_db:
      self._CreateKeyDB(create_if_necessary)

  def _CreateKeyDB(self, create_if_necessary=False):
    self._keydb = SimpleDB(self._keydb_path, encode_keys=False)
    self._keydb.Open(create_if_necessary=create_if_necessary)

    # Initialize id to 1 if not found
    try:
      self._keydb.GetFirst(NEXT_ID_KEY)
    except RowNotFound:
      self._keydb.Set(NEXT_ID_KEY, 1, 0)

  def _CreateBDB(self):
    if os.path.exists(self._db_path):
      raise DBExists("Refusing to overwrite existing file: %s" % self._db_path)
    handle = db.DB()
    handle.set_pagesize(4096)
    handle.open(self._db_path, None, BDB_ACCESS_FLAGS, db.DB_CREATE);
    handle.close()

  def _OpenBDB(self):
    self._dbh.open(self._db_path, None,
                   BDB_ACCESS_FLAGS, db.DB_DIRTY_READ)

  def PurgeDatabase(self):
    if self._dbh:
      self._dbh.close()
    if os.path.exists(self._db_path):
      os.unlink(self._db_path)

    if self.use_key_db:
      if self._keydb:
        self._keydb.PurgeDatabase()
      if os.path.exists(self._keydb_path):
        os.unlink(self._keydb_path)

  def GenerateDBKey(self, row, timestamp=None):
    if timestamp is None:
      timestamp = time.time() * 1000000
    elif isinstance(timestamp, float):
      # if we've been given a float assume it's time.time()'s output
      timestamp *= 1000000
    timestamp = long(timestamp)
    return self.GenerateDBKeyWithTimestamp(row, timestamp)

  def GenerateDBKeyWithTimestamp(self, row, timestamp):
    if self.use_key_db:
      row = self.GetRowID(row, create_if_necessary=True)
    #if "keys" not in self._db_path:
      #print "%s @ %s" % (row, timestamp)

    timestamp = (1<<63) - timestamp
    return "%s%s" % (row, To64(timestamp))

  def GetRowID(self, row, create_if_necessary=False):
    if not self.use_key_db:
      return row

    id = None
    try:
      record = self._keydb.GetFirst(row)
      if record:
        id = record.value
    except RowNotFound:
      pass

    if id is None:
      if create_if_necessary:
        id = self.CreateRowID(row)
      else:
        raise RowNotFound("Row '%s' not known to database" % row)
    return id

  def GetRowByID(self, id):
    if not self.use_key_db:
      return id
    record = self._keydb.GetFirst(id)
    if record:
      return record.value
    return None

  def CreateRowID(self, row):
    if not self.use_key_db:
      return row
    next_id = self._keydb.GetFirst(NEXT_ID_KEY).value
    id = To64(next_id)
    # Map row => id, and id => row
    self._keydb.Set(row, id, 0)
    self._keydb.Set(id, row, 0)
    self._keydb.Set(NEXT_ID_KEY, next_id + 1, 0)
    return id

  def Set(self, row, value, timestamp=None):
    assert row is not None
    key = self.GenerateDBKey(row, timestamp)
    value = cPickle.dumps(value)
    self._dbh[key] = value

  def Delete(self, row, timestamp):
    key = self.GenerateDBKey(row, timestamp)
    cursor = self._dbh.cursor()
    cursor.set(key)
    cursor.delete()

  def DeleteRow(self, row):
    for entry in self.ItemIteratorByRows([row]):
      self.Delete(entry.row, entry.timestamp)

  def ItemIterator(self, start_row=""):
    cursor = self._dbh.cursor()
    if start_row:
      start_row = self.GetRowID(start_row)
    record = cursor.set_range(start_row)
    while record:
      (key, value) = record
      (row, timestamp) = KeyToRowAndTimestamp(key)
      timestamp = (1<<63) - timestamp
      try:
        row = self.GetRowByID(row)
      except RowNotFound:
        print "Failed finding row '%s'" % row
      value = cPickle.loads(record[1])
      yield Entry(row, timestamp, value)
      record = cursor.next()
    cursor.close()

  def GetFirst(self, row):
    iterator = self.ItemIterator(row)
    try:
      entry = iterator.next()
    except StopIteration:
      raise RowNotFound(row)

    if entry.row == row:
      return entry
    raise RowNotFound(row)

  def ItemIteratorByRows(self, rows=[]):
    for row in rows:
      start = row
      end = EndRow(row)
      for entry in self.ItemIterator("%s" % start):
        if entry.row != row:
          continue
        if entry.row > row:
          break
        yield entry

  def ItemIteratorByRowRange(self, start_row, end_row):
    for entry  in self.ItemIterator(start_row):
      if entry.row > end_row:
        break;
      yield entry

  def Clean(self):
    self._RemoveUnusedKeys()

  def _RemoveUnusedKeys(self):
    if not self.use_key_db:
      return

    iterator = self._keydb.ItemIterator()
    rows_to_remove = []
    for entry in iterator:
      if entry.row == NEXT_ID_KEY:
        continue
      failcount = 0
      for row in (entry.value, entry.row):
        try:
          item = self.GetFirst(row)
        except RowNotFound:
          failcount += 1
      if failcount == 2:
        rows_to_remove.extend((entry.row, entry.value))
    for row in rows_to_remove:
      self._keydb.DeleteRow(row)

  def FetchAggregate(self, rows, time_bucket, aggregate_func):
    data = {}
    for entry in self.ItemIteratorByRows(rows):
      bucket = entry.timestamp - (entry.timestamp % time_bucket)
      data.setdefault(bucket, [])
      data[bucket].append(entry.value)

    aggregates = []
    keys = sorted(data.keys())
    for key in keys:
      aggregates.append((key, aggregate_func(data[key])))

    return aggregates

class DictDB(SimpleDB):
  def __getitem__(self, key):
    try:
      iterator = self.ItemIteratorByRows([key])
      return iterator.next().value
    except RowNotFound:
      raise KeyError, key

  def __setitem__(self, key, value):
    self.Set(key, value, 0)

  def __delitem__(self, key):
    self.Delete(key)


def test():
  f = "/tmp/test2"
  SimpleDB(f).PurgeDatabase()
  x = SimpleDB(f)
  x.Open(create_if_necessary=True)

  for i in range(10):
    x.Set("foo", i)
  for i in range(10):
    x.Set("test", i)

  x.Set("one", 1, 1)
  x.Delete("one", 1)

  x.DeleteRow("foo")
  x.DeleteRow("test")

  for i in x.ItemIterator():
    print "%s[%s]: %s" % (i.row, i.timestamp, i.value)

def score():
  f = "/tmp/test2"
  SimpleDB(f).PurgeDatabase()
  db = SimpleDB(f)
  db.Open(create_if_necessary=True)

  import subprocess
  for x in range(5):
    print "Round: %d" % x
    output = subprocess.Popen(["netstat", "-s"], stdout=subprocess.PIPE).communicate()[0]
    for i in output.split("\n"):
      i = i.strip()
      fields = i.split()
      if "incoming packets delivered" in i:
        db.Set("ip.in.packets", int(fields[0]))
      elif "requests sent out" in i:
        db.Set("ip.out.packets", int(fields[0]))
    time.sleep(1)

  for i in db.ItemIterator():
    print i

def testclean():
  f = "/tmp/test2"
  SimpleDB(f).PurgeDatabase()
  db = SimpleDB(f)
  db.Open(create_if_necessary=True)

  print "set"
  db.Set("foo", "bar", 0)
  db.Set("foo2", "bar2", 0)
  #print [x for x in db.ItemIterator()]
  print "del"
  db.Delete("foo", 0)
  #print [x for x in db.ItemIterator()]
  print "clean"
  db.Clean()
  print [x for x in db.ItemIterator()]
  print [x for x in db._keydb.ItemIterator()]

def proftest():
  f = "/tmp/test2"
  SimpleDB(f).PurgeDatabase()
  db = SimpleDB(f, encode_keys=True)
  db.Open(create_if_necessary=True)
  iterations = 10

  db.Set("one", 1)
  db.Set("two", 2)

  print "Set foo/bar/baz"
  for i in range(iterations):
    db.Set("foo", i)
    db.Set("bar", i)
    db.Set("baz", i)

  #print "Set i"
  #for i in range(100):
    ###for j in range(100):
      #db.Set(str(i), j, j)

  print "Scan"
  for i in db.ItemIterator():
    print i

  print "Delete"
  db.DeleteRow("bar")
  db.DeleteRow("one")
  db.DeleteRow("two")

  print "Clean"
  db.Clean()
  print "Done"

def profile(func, *args):
  import hotshot
  output = "/tmp/my.profile"
  p = hotshot.Profile(output)
  p.runcall(func, *args)
  p.close()

#testclean()

#profile(proftest)
#proftest()

def cachetest():
  x = Cache(5)

  for i in range(100):
    x[i] = i

  print x
