#!/usr/bin/env python

import mytest
import simpledb
import tempfile
import logging

class SimpleDBTest(mytest.EnhancedTestCase):
  def setUp(self):
    self._dbfile = tempfile.mkstemp()[1]
    logging.basicConfig(
      level=logging.INFO,
      format="%(asctime)s %(levelname)s %(message)s",
      filename="/tmp/testlog"
      )

    self.db = simpledb.SimpleDB(self._dbfile)
    self.db.Open(create_if_necessary=True)

  def tearDown(self):
    self.db.PurgeDatabase()

  def testGetNewest(self):
    self.db.Set("foo", "one", 0)
    self.db.Set("foo", "two", 1)
    self.db.Set("foo", "three", 2)
    entry = self.db.GetNewest("foo")
    self.assertEquals(entry.value, "three")

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

if __name__ == "__main__":
  mytest.main()
