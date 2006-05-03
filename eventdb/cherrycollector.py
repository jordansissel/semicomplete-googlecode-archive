#!/usr/local/bin/python

from threading import Thread
from pysqlite2 import dbapi2 as sqlite
from Queue import Queue
import threading
import os
import cherrypy
import time

class EventCollectionServer:
	def store(self, where, what, when=time.time()):
		if (where is None):
			return "No 'where' parameter passed?"

		EventsDB.instance.add({ "where": where, "what": what, "when": when})
		return "OK"
	store.exposed = True

class EventsDB(Thread):
	instance = None
	def __init__(self):
		self.queue = Queue()
		self.dbpath = "./events.db"
		Thread.__init__(self, name="EventsDB")
		EventsDB.instance = self

	def run(self):
		self.db = sqlite.connect(self.dbpath) #, detect_types=sqlite.PARSE_DECLTYPES)
		self.db.isolation_level = None
		self.db.execute("BEGIN")
		while 1:
			event = self.queue.get()
			print "Got event type: %s" % (event["type"])
			if event["type"] == "event":
				print "Got an event: %s" % (event)
				self._store(event["data"])
			elif event["type"] == "command":
				self._command(event["data"])

	def _command(self, cmd):
		if cmd == "sync":
			print "Synchronizing with file system"
			self.db.execute("COMMIT")
			self.db.execute("BEGIN")

	def _store(self,event):
		if not threading.currentThread() == self:
			print "You cannot call this function from another thread"
			return

		c = self.db.cursor()
		query = "INSERT INTO events (time, location, data) VALUES (:when, :where, :what)"
		c.execute(query, event)
		c.close()
	
	def add(self, data):
		self.queue.put({"type": "event", "data": data})

	def sync(self):
		self.queue.put({"type": "command", "data": "sync"})

class SyncDB(Thread):
	def __init__(self):
		Thread.__init__(self, name="SyncDB")
	def run(self):
		try:
			while 1:
				time.sleep(5)
				EventsDB.instance.sync()
		except SystemExit:
			print "Syncer dying"

if __name__ == "__main__":
	#if EventsDB.instance is None:
	if os.environ.has_key("WEBSTART"):
		print "Starting other threads"
		edb = EventsDB()
		edb.start()

		syncer = SyncDB()
		syncer.start()

	os.environ["WEBSTART"] = "1" 

	ecs = EventCollectionServer()
	cherrypy.root = ecs
	cherrypy.root.index = ecs.store
	try:
		cherrypy.server.start(init_only=True)
	except:
		# Let's mark that we are done, so the other threads can be notified?
		print "CherryPy died."
