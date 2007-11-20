#!/usr/bin/python

from bsddb import db

foo = db.DB()
foo.open("/tmp/test.db", None, db.DB_BTREE, db.DB_CREATE)

foo.put("a", "bad")
foo.put("foo", "bar")
foo.put("foo2", "bar2")
foo.put("foo3", "bar3")
foo.close()

foo = db.DB()
foo.open("/tmp/test.db", None, db.DB_BTREE, db.DB_DIRTY_READ)
c = foo.cursor();
c.set("foo", flags=db.DB_SET)
rec = c.next()
while rec:
  print rec
  rec = c.next()

foo.close()

