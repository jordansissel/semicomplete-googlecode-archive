#!/usr/bin/python

from bsddb import db

import os
import time
import struct
import cPickle

BDB_ACCESS_FLAGS = db.DB_BTREE

class DBExists(Exception):
  pass

class RecDB(object):

  def __init__(self, db_path, create_if_necessary=False):
    self._db_path = db_path
    if create_if_necessary and not os.path.exists(self._db_path):
      self.CreateBDB()

    self._dbh = db.DB()
    self.OpenBDB()

  def CreateBDB(self):
    if os.path.exists(self._db_path):
      raise DBExists("Refusing to overwrite existing file: %s" % self._db_path)

    handle = db.DB()
    handle.open(self._db_path, None, BDB_ACCESS_FLAGS, db.DB_CREATE);
    handle.close()

  def OpenBDB(self):
    self._dbh.open(self._db_path, None, BDB_ACCESS_FLAGS, db.DB_DIRTY_READ)

  def GenerateDBKey(self, row, timestamp=None):
    if timestamp is None:
      timestamp = time.time() * 1000000
    return self.GenerateDBKeyWithTimestamp(row, timestamp)

  def GenerateDBKeyWithTimestamp(self, row, timestamp):
    return "%s.%d" % (row, timestamp)

  def Set(self, key, value, timestamp=None):
    assert key is not None
    dbkey = self.GenerateDBKeyWithTimestamp(key, timestamp)
    value = cPickle.dumps(value)
    self._dbh.put(dbkey, value)

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
    record = cursor.set_range(start_row)
    while record:
      value = cPickle.loads(record[1])
      yield (record[0], value)
      record = cursor.next()
    cursor.close()

  def ItemIteratorByRowRange(self, start_row, end_row):
    for (row, value) in self.ItemIterator(start_row):
      if row > end_row:
        break;
      yield(row, value)

f = "/tmp/test2.db"
if os.path.exists(f):
  os.unlink(f)
x = RecDB(f, create_if_necessary=True)

for i in range(10000):
  x.Add("test", 1, timestamp=0)
for i in x.ItemIterator():
  print i
