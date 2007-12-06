#!/usr/bin/env python

import bsddb.db as bdb

def populate(db):
  for i in range(200000):
    db[str(i % 1000)] = "foo"

def scan(db):
  c = db.cursor()
  record = c.first()
  while record:
    if record[1] != "foo":
      print "Unexpected result: %s" % (record,)
    record = c.next()


dbfile = "/tmp/python.db"
db = bdb.DB()
db.set_pagesize(4096)
db.open(dbfile, None, bdb.DB_BTREE, bdb.DB_CREATE)

populate(db)
scan(db)
