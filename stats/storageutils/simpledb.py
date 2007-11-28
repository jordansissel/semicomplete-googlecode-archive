#!/usr/bin/env python

from bsddb import db as bdb

import os
import time
import struct
import cPickle
import threading
import Queue

# database:
# timestamp should be 64bit value: epoch in milliseconds == 52 bits.
# row should be 64bit id
# key = row + timestamp

# if row -> id not exists, add to database

BDB_ACCESS_FLAGS = bdb.DB_BTREE
NEXT_ID_KEY = "_next_id"
TIMESTAMP_MAX = (1<<63)

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

# Do we need this?
def EndRow(row):
  x = ord(row[-1])
  if x == 255:
    return EndRow("%s\0" % row[:-1])
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

def synchronized_method(func):
  def newfunc(self, *args, **kwds):
    assert(hasattr(self, "_lock"))
    self._lock.acquire()
    func(self, *args, **kwds)
    self._lock.release()
  newfunc.__name__ = "(synchronized)%s" % func.__name__
  newfunc.__doc__ = func.__doc__
  newfunc.__dict__.update(func.__dict__)

class DBDictCursor(object):
  def __init__(self, data):
    self._data = data
    self._keys = data.keys()
    self._index = 0

  def close(self):
    pass

  def delete(self):
    del self._data[self._keys[self._index]]

  def set(self, key):
    index = 0
    for i in self._keys:
      if i == key:
        self._index = index
        return self._CurrentPair()
      index += 1
    raise bdb.DBNotFoundError, key

  def set_range(self, key):
    index = 0
    for i in self._keys:
      if i >= key:
        self._index = index
        return self._CurrentPair()
        return
      index += 1

  def _CurrentPair(self):
    key = self._keys[self._index]
    return (key, self._data[key])

  def next(self):
    self._index += 1
    if self._index >= len(self._keys):
      #raise bdb.DBNotFoundError, "End of database reached."
      return None
    return self._CurrentPair()

class DBDict(dict):
  def cursor(self):
    return DBDictCursor(self)

  def truncate(self):
    self.clear()

  def close(self):
    pass


class FlushWorker(threading.Thread):
  def __init__(self, simpledb):
    threading.Thread.__init__(self)
    self._queue = Queue.Queue()
    self._data_lock = threading.Condition()
    self._done = threading.Event()
    self._done.clear()
    self._simpledb = simpledb

  def AddDatabaseToFlushQueue(self, dbh):
    self._data_lock.acquire()
    self._queue.put(dbh)
    self._data_lock.notifyAll()
    self._data_lock.release()

  def run(self):
    done = False
    dbh = None
    while not done:
      self._data_lock.acquire()
      self._data_lock.wait()
      size = self._queue.qsize()
      self._data_lock.release()

      for i in range(size):
        dbh = self._queue.get()
        #self._simpledb.FlushMemoryDatabase(dbh)
        self.FlushDBH(dbh)
      done = self._done.isSet()

  def FlushDBH(self, dbh):
    print ":: flushing memory database %s" % ( threading.currentThread())
    mydb = self._simpledb
    for entry in mydb._db_ItemIterator(dbh):
      #print "%s/%s %s" % (threading.currentThread(), dbh, entry)
      mydb._db_Set(mydb._dbh, entry.row, entry.value, entry.timestamp)
    dbh.truncate()
    dbh.close()
    del dbh

  def Finish(self):
    print "finish"
    self._done.set()
    self._data_lock.acquire()
    self._data_lock.notifyAll()
    self._data_lock.release()

class SimpleDB(object):
  """ Simple timestamped key-value pair storage space. 

  Backed with BDB.
  """

  def __init__(self, db_root, db_name="main", encode_keys=False):
    self._db_root = db_root
    self._db_name = db_name
    self._db_path = "%s/%s" % (self._db_root, self._db_name)
    self._dbh = None
    self._memory_dbh = None
    self._dbenv = None
    self._memory_db_size = 0
    self.MAX_MEMORY_DB_SIZE = 500<<20
    self._flushlock = threading.RLock()
    self._flushworker = FlushWorker(self)
    self._flushworker.start()

    self.use_key_db = encode_keys
    if self.use_key_db:
      self._keydb_path = "%s/keys" % self._db_root
      self._keydb = None

  def _SetDBEnv(self, dbenv):
    self._dbenv = dbenv

  def _GetDBEnv(self):
    return self._dbenv

  def Open(self, create_if_necessary=False):
    # XXX: Make sure this is the correct way to do this.
    if not os.path.isdir(self._db_root) and create_if_necessary:
      os.mkdir(self._db_root)

    if type(self._dbenv) != "DBEnv":
      self._dbenv = bdb.DBEnv()
      self._dbenv.set_cachesize(0, 20<<20)
      flags = bdb.DB_INIT_LOCK | bdb.DB_INIT_MPOOL \
              | (create_if_necessary and bdb.DB_CREATE)
      self._dbenv.open(self._db_root, flags)

    if create_if_necessary:
      self._CreateBDB()

    self._OpenBDB()
    self._OpenMemoryBDB()

    if self.use_key_db:
      self._CreateKeyDB(create_if_necessary)

  def FlushMemoryDatabase(self):
    self._flushlock.acquire()
    old_memory_dbh = self._memory_dbh
    self._OpenMemoryBDB()
    self._flushlock.release()
    self._flushworker.AddDatabaseToFlushQueue(old_memory_dbh)

  def Close(self):
    self.FlushMemoryDatabase()
    self._flushworker.Finish()
    self._flushworker.join()
    print "simpledb: closing _dbh"
    self._dbh.close()
    print "simpledb: closed _dbh"
    if self.use_key_db:
      self._keydb.close()
    print "Done"

  def _CreateKeyDB(self, create_if_necessary=False):
    self._keydb = SimpleDB(self._db_root, db_name="keys", encode_keys=False)
    self._keydb._SetDBEnv(self._dbenv)
    self._keydb.Open(create_if_necessary=create_if_necessary)

    # Initialize id to 1 if not found
    try:
      self._keydb.GetNewest(NEXT_ID_KEY)
    except RowNotFound:
      self._keydb.Set(NEXT_ID_KEY, 1, 0)

  def _CreateBDB(self):
    #if os.path.exists(self._db_root):
      #raise DBExists("Refusing to overwrite existing file: %s" % self._db_path)
    handle = bdb.DB(self._dbenv)
    handle.set_pagesize(4096)
    handle.open(self._db_path, self._db_name, BDB_ACCESS_FLAGS, bdb.DB_CREATE);
    handle.close()

  def _OpenBDB(self):
    self._dbh = bdb.DB(self._dbenv)
    self._dbh.open(self._db_path, self._db_name, BDB_ACCESS_FLAGS, 0)

  def _OpenMemoryBDB(self):
    #self._memory_dbh = bdb.DB(self._dbenv)
    #self._memory_dbh.open(None, None, BDB_ACCESS_FLAGS, bdb.DB_CREATE)
    self._memory_dbh = DBDict()

  def PurgeDatabase(self):
    if self._dbh:
      self._dbh.close()
    if os.path.exists(self._db_path):
      os.unlink(self._db_path)
    if self.use_key_db and self._keydb:
      self._keydb.PurgeDatabase()

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

    timestamp = TIMESTAMP_MAX - timestamp
    return "%s%s" % (row, To64(timestamp))

  def GetRowID(self, row, create_if_necessary=False):
    if not self.use_key_db:
      return row

    id = None
    try:
      record = self._keydb.GetNewest(row)
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
    record = self._keydb.GetNewest(id)
    if record:
      return record.value
    return None

  def CreateRowID(self, row):
    if not self.use_key_db:
      return row
    next_id = self._keydb.GetNewest(NEXT_ID_KEY).value
    id = To64(next_id)
    # Map row => id, and id => row
    self._keydb.Set(row, id, 0)
    self._keydb.Set(id, row, 0)
    self._keydb.Set(NEXT_ID_KEY, next_id + 1, 0)
    return id

  def Set(self, row, value, timestamp=None):
    # XXX: This should be done in a separate thread.
    bytes = self._db_Set(self._memory_dbh, row, value, timestamp)

    self._memory_db_size += bytes
    if self._memory_db_size >= self.MAX_MEMORY_DB_SIZE:
      print "Bytes: %d" % self._memory_db_size
      self.FlushMemoryDatabase()
      self._memory_db_size = 0

  def _db_Set(self, dbh, row, value, timestamp=None):
    assert row is not None
    key = self.GenerateDBKey(row, timestamp)
    value = cPickle.dumps(value)
    dbh[key] = value
    return len(value) + len(row) + 8

  def Delete(self, row, timestamp):
    key = self.GenerateDBKey(row, timestamp)
    cursor = self._dbh.cursor()
    cursor.set(key)
    cursor.delete()

  def DeleteRow(self, row):
    for entry in self.ItemIteratorByRows([row]):
      self.Delete(entry.row, entry.timestamp)

  def GetNewest(self, row):
    iterator = self.ItemIterator(row)
    try:
      entry = iterator.next()
    except StopIteration:
      raise RowNotFound(row)

    if entry.row == row:
      return entry
    raise RowNotFound(row)

  def ItemIterator(self, start_row="", start_timestamp=TIMESTAMP_MAX, 
                   end_timestamp=0):
    for dbh in (self._memory_dbh, self._dbh):
      for entry in self._db_ItemIterator(dbh, start_row, start_timestamp, 
                                         end_timestamp):
        yield entry

  def _db_ItemIterator(self, dbh, start_row="", start_timestamp=TIMESTAMP_MAX,
                       end_timestamp=0):
    cursor = dbh.cursor()
    if start_row:
      start_row = self.GetRowID(start_row)

    if end_timestamp > start_timestamp:
      start_timestamp, end_timestamp = end_timestamp, start_timestamp

    start_row = self.GenerateDBKeyWithTimestamp(start_row, start_timestamp)
    record = cursor.set_range(start_row)
    while record:
      (key, value) = record
      (row, timestamp) = KeyToRowAndTimestamp(key)
      timestamp = TIMESTAMP_MAX - timestamp
      if timestamp < end_timestamp:
        break
      try:
        row = self.GetRowByID(row)
      except RowNotFound:
        print "Failed finding row '%s'" % row
      value = cPickle.loads(record[1])
      yield Entry(row, timestamp, value)
      record = cursor.next()
    try:
      cursor.close()
    except bdb.DBInvalidArgError, e:
      print "Error when closing cursor: %s" % e

  def ItemIteratorByRows(self, rows=[], start_timestamp=TIMESTAMP_MAX, end_timestamp=0):
    for row in rows:
      for entry in self.ItemIterator(row, start_timestamp, end_timestamp):
        #print "ItemRowIter: %s vs %s" % (row, entry.row)
        if entry.row != row:
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
          item = self.GetNewest(row)
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

def test():
  import tempfile
  tmpdir = tempfile.mkdtemp()
  d = SimpleDB(tmpdir)
  d.Open(create_if_necessary=True)
  for i in range(1000000):
    if i % 10000 == 0:
      print i
    d.Set("foo", i, i)
  for i in d.ItemIterator():
    print i

#test()
