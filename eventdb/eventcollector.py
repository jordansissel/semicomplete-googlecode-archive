
import threading
from threading import Thread, Event
from pysqlite2 import dbapi2 as sqlite
from Queue import Queue
import time
import sys
import SocketServer, BaseHTTPServer
import cgi

class EventCollectionServer(Thread):
	def __init__(self, port=5234):
		self.port = port
		self.server = SocketServer.ThreadingTCPServer(('', self.port), self.handler)
		Thread.__init__(self, name="EventCollectionServer")

	def run(self):
		while 1:
			self.server.handle_request()

	class handler(BaseHTTPServer.BaseHTTPRequestHandler):

		def do_POST(self):
			#self.query_string = self.rfile.read(int(self.headers['Content-Length']))
			self.respond()

		def do_GET(self):
			if self.path.find('?') != -1:
				self.path, self.query_string = self.path.split('?', 1)
			else:
				self.query_string = ''
			self.respond()

		def respond(self):
			ok = 1
			if self.headers.gettype() == "text/plain":
				self.args = dict(cgi.parse_qsl(self.query_string))
			else:
				sys.stdin = self.rfile
				self.args = cgi.FieldStorage()
			self.messages = []

			self.send_response(200, "OK")
			self.end_headers()

			print "args: %s" % self.args
			for i in self.args:
				print "%s: %s" % (i, self.args[i])
			map(lambda x: self.wfile.write("%s\n" % (x)), self.messages)
			if (not self.args.has_key("where") or
				 not self.args.has_key("what")):
				self.messages.append("Invalid parameters")
				ok = 0

			if ok:
				self.addevent(self.args)

		def addevent(self, args):
			if not args.has_key("when"):
				args["when"] = time.time()
			EventsDB.instance.add(args)


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
				self.store(event["data"])
			elif event["type"] == "command":
				self.command(event["data"])

	def command(self, cmd):
		if cmd == "sync":
			print "Synchronizing with file system"
			self.db.execute("COMMIT")
			self.db.execute("BEGIN")

	def store(self,event):
		if not threading.currentThread() == self:
			print "You cannot call this function from another thread"
			return

		c = self.db.cursor()
		#data = (event["when"], event["where"], event["what"])

		#query = "INSERT INTO events (time, location, data) VALUES (?, ?, ?)"
		#c.execute(query, data)
		#self.db.execute(query, data)
		query = "INSERT INTO events (time, location, data) VALUES (:when, :where, :what)"
		c.execute(query, event)

		c = self.db.cursor()
		c.execute("SELECT * FROM events")
		print c.fetchall()
	
	def add(self, data):
		self.queue.put({"type": "event", "data": data})

	def sync(self):
		self.queue.put({"type": "command", "data": "sync"})

edb = EventsDB()
server = EventCollectionServer()

edb.start()
server.start()

EventsDB.instance.store({"test": 1234})

print "Waiting for thread to die"

while 1:
	time.sleep(5)
	EventsDB.instance.sync()

