#!/usr/bin/env python

import mytest
import simpledb
import tempfile
import logging
import os

def mktmpdir():
  return tempfile.mkdtemp()

class SimpleDBTestBasics(mytest.EnhancedTestCase):
  def setUp(self):
    self._db_root = mktmpdir()
    logging.basicConfig(
      level=logging.INFO,
      format="%(asctime)s %(levelname)s %(message)s",
      filename="/tmp/testlog"
      )

    self.db = simpledb.SimpleDB(self._db_root)
    self.db.Open(create_if_necessary=True)

  def tearDown(self):
    self.db.PurgeDatabase()
    for i in os.listdir(self._db_root):
      os.unlink("%s/%s" % (self._db_root, i))
    os.rmdir(self._db_root)

  def testGetNewest(self):
    #os.environ["DEBUG"] = ""
    self.db.Set("foo", "one", 0)
    self.db.Set("foo", "two", 1)
    self.db.Set("foo", "three", 2)
    entry = self.db.GetNewest("foo")
    self.assertEquals(entry.value, "three")
    #del os.environ["DEBUG"]

  def testIterationOrderIsNewestTimestampFirst(self):
    self.db.Set("foo", "b", 30)
    self.db.Set("foo", "c", 10)
    self.db.Set("foo", "a", 20)
    # order should be (b@30, a@20, c@10)
    values = [x.value for x in self.db.ItemIterator()]
    self.assertListEquals(values, ("b", "a", "c"))

  def testIterationByRowsOnlyReturnsSpecificRows(self):
    for i in range(10):
      self.db.Set("fire", i, i)
      self.db.Set("firetruck", i, i)
      self.db.Set("test", i, i)

    rows = ("fire", "firetruck", "test")
    for row in rows:
      for entry in self.db.ItemIteratorByRows([row]):
        self.assertEquals(entry.row, row)

  def testDataPersistsAcrossInstances(self):
    self.db.Set("test", "foo", 0)
    self.assertEquals(self.db.GetNewest("test").value, "foo")
    self.db.Close()
    self.db.Open()
    self.assertEquals(self.db.GetNewest("test").value, "foo")

  def testTimestampRestrictedIteration(self):
    for i in range(20):
      self.db.Set("test", i, i)

    expected = (
      "test@10:10",
      "test@9:9",
      "test@8:8",
      "test@7:7",
      "test@6:6",
      "test@5:5",
    )
    results = []
    for x in self.db.ItemIteratorByRows(["test"], 5, 10):
      results.append("%s@%d:%d" % (x.row, x.timestamp, x.value))

    self.assertListEquals(expected, results)

  def testDeleteEntry(self):
    row = "foo"
    self.db.Set(row, "bar", 1)
    self.assertEquals(self.db.GetNewest(row).value, "bar")
    self.db.Delete(row, 1)
    def x():
      self.db.GetNewest(row)
    self.assertRaises(simpledb.RowNotFound, x)

  def testDeleteRow(self):
    return True
    row = "foo"
    self.db.Set(row, "bar", 100)
    self.db.Set(row, "baz", 99)
    for i in range(10):
      self.db.Set(row, i, i)
    self.assertEquals(self.db.GetNewest(row).value, "bar")
    self.db.Delete(row, )
    self.assertEquals(self.db.GetNewest(row).value, "baz")
    self.db.DeleteRow(row)
    def x():
      self.db.GetNewest(row)
    self.assertRaises(simpledb.RowNotFound, x)

class SimpleDBTestBasicsWithKeyEncoding(SimpleDBTestBasics):
  def setUp(self):
    self._db_root = mktmpdir()
    logging.basicConfig(
      level=logging.INFO,
      format="%(asctime)s %(levelname)s %(message)s",
      filename="/tmp/testlog"
      )

    self.db = simpledb.SimpleDB(self._db_root, encode_keys=True)
    self.db.Open(create_if_necessary=True)

  def testKeysDBExists(self):
    self.assert_(self.db._keydb)

  def testKeyDBInsertion(self):
    row = "foo"
    self.db.Set(row, "bar")
    entry = self.db._keydb.GetNewest(row)
    self.assertEquals(entry.row, row)
    self.assertEquals(entry.value, simpledb.To64(1))

if __name__ == "__main__":
  mytest.main()
