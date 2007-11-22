#!/usr/bin/python

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

class DBExists(Exception):
  pass

class RowNotFound(Exception):
  pass

# use big endian packing since little endian sucks at sorting.
def To64(num):
  return struct.pack(">q", long(num))

def From64(data):
  return struct.unpack(">q", data)[0]

def KeyToRowAndTimestamp(key):
  #return struct.unpack("qq", key)
  return (key[:8], From64(key[8:]))

class Entry(object):
  def __init__(self, row, timestamp, value):
    self.row = row
    self.timestamp = timestamp
    self.value = value

  def __str__(self):
    return "%s[%s]: %s" % (self.row, self.timestamp, self.value)

class RecDB(object):

  def __init__(self, db_path, create_if_necessary=False):
    self._db_path = db_path
    self._keys_db_path = "%s.keys" % db_path
    if create_if_necessary and not os.path.exists(self._db_path):
      self.CreateBDB()

    self._dbh = None
    self._keys_dbh = None

  def Open(self):
    self._dbh = db.DB()
    self._keys_dbh = db.DB()
    self.OpenBDB()

  def CreateBDB(self):
    if os.path.exists(self._db_path):
      raise DBExists("Refusing to overwrite existing file: %s" % self._db_path)
    handle = db.DB()
    handle.set_pagesize(4096)
    handle.open(self._db_path, None, BDB_ACCESS_FLAGS, db.DB_CREATE);
    handle.close()

    if os.path.exists(self._keys_db_path):
      raise DBExists("Refusing to overwrite existing file: %s" % self._keys_db_path)
    handle = db.DB()
    handle.set_pagesize(4096)
    handle.set_cachesize(0, 20<<20) # 20 meg cache
    handle.open(self._keys_db_path, None, BDB_ACCESS_FLAGS, db.DB_CREATE);
    handle.close()

  def PurgeDatabase(self):
    if self._dbh:
      self._dbh.close()
    if self._keys_dbh:
      self._keys_dbh.close()
    if os.path.exists(self._db_path):
      os.unlink(self._db_path)
    if os.path.exists(self._keys_db_path):
      os.unlink(self._keys_db_path)

  def OpenBDB(self):
    self._dbh.open(self._db_path, None,
                   BDB_ACCESS_FLAGS, db.DB_DIRTY_READ)
    self._keys_dbh.open(self._keys_db_path, None,
                        BDB_ACCESS_FLAGS, db.DB_DIRTY_READ)

  def GenerateDBKey(self, row, timestamp=None):
    if timestamp is None:
      timestamp = time.time() * 1000000
    return self.GenerateDBKeyWithTimestamp(row, timestamp)

  def GenerateDBKeyWithTimestamp(self, row, timestamp):
    row = self.GetRowID(row, create_if_necessary=True)
    return "%s%s" % (row, To64(timestamp))

  def GetRowID(self, row, create_if_necessary=False):
    cursor = self._keys_dbh.cursor()
    record = cursor.set(row)
    cursor.close()
    if record:
      id = record[1]
    elif create_if_necessary:
      id = self.CreateRowID(row)
    else:
      raise RowNotFound("Row '%s' not known to database" % row)
    return id

  def GetRowByID(self, id):
    cursor = self._keys_dbh.cursor()
    record = cursor.set(id)
    cursor.close()
    if record:
      return record[1]
    return None

  def CreateRowID(self, row):
    cursor = self._keys_dbh.cursor()
    last_record = cursor.last()
    if last_record is None:
      id = 1
    else:
      id = From64(last_record[1]) + 1
    id = To64(id)
    # Map row => id, and id => row
    self._keys_dbh.put(row, id)
    self._keys_dbh.put(id, row)
    return id

  def Set(self, row, value, timestamp=None):
    assert row is not None
    key = self.GenerateDBKey(row, timestamp)
    value = cPickle.dumps(value)
    self._dbh.put(key, value)

  def Add(self, key, value, timestamp=None):
    dbkey = self.GenerateDBKey(key, timestamp)
    try:
      iter = self.ItemIterator(dbkey)
      (old_key, old_value) = iter.next()
      value += old_value
    except StopIteration:
      pass
    self.Set(key, value, timestamp)

  def ItemIterator(self, start_row=""):
    cursor = self._dbh.cursor()
    if start_row:
      start_row = self.GetRowID(start_row)
    record = cursor.set_range(start_row)
    while record:
      (key, value) = record
      (row, timestamp) = KeyToRowAndTimestamp(key)
      row = self.GetRowByID(row)
      value = cPickle.loads(record[1])
      yield Entry(row, timestamp, value)
      record = cursor.next()
    cursor.close()

  def ItemIteratorByRows(self, rows=[]):
    cursor = self._dbh.cursor()
    for row in rows:
      #row = self.GetRowID(row)
      for entry in self.ItemIterator(row):
        if entry.row != row:
          break;
        yield entry

  def ItemIteratorByRowRange(self, start_row, end_row):
    for entry  in self.ItemIterator(start_row):
      if entry.row > end_row:
        break;
      yield entry

def test():
  f = "/tmp/test2"
  RecDB(f, create_if_necessary=True).PurgeDatabase()
  x = RecDB(f, create_if_necessary=True)
  x.Open()

  for i in range(1000):
    x.Set("foo", i)
  for i in range(1000):
    x.Set("test", i)

  for z in range(30):
    for i in x.ItemIteratorByRows(["foo"]):
      print "%s[%s]: %s" % (i.row, i.timestamp, i.value)

def score():
  f = "/tmp/test2"
  RecDB(f, create_if_necessary=True).PurgeDatabase()
  db = RecDB(f, create_if_necessary=True)
  db.Open()

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


test()
