#!/usr/bin/env python

import bsddb.db as bdb
import sys

size = int(sys.argv[1])
def populate(db):
  for i in range(size):
    db[str(i)] = "foo"

def scan(db):
  c = db.cursor()
  record = c.first()
  count = 0
  while record:
    count += 1
    if record[1] != "foo":
      print "Unexpected result: %s" % (record,)
    record = c.next()

  if count != size:
    print "Size mismatch on full read: %d vs %d" % (count, size)


dbfile = "/tmp/python.db"
db = bdb.DB()
db.set_pagesize(int(sys.argv[2]))
db.set_cachesize(0, int(sys.argv[3]))
db.open(dbfile, None, bdb.DB_BTREE, bdb.DB_CREATE)

#populate(db)
scan(db)
