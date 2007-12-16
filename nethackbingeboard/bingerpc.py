import BaseHTTPServer
import Queue
import SocketServer
import select
import socket
import os
import struct
import sys
import threading
import time
import cgi

import simplejson

DEFAULT_BOARD_PORT = 15030

DM_BOARD_KEEPALIVE      = 0x00
DM_BOARD_BINGE          = 0x01
DM_BOARD_PLAYER_NAME    = 0x02
DM_BOARD_STATUS         = 0x03
DM_BOARD_PLAYER         = 0x04
DM_BOARD_STAT           = 0x05
DM_BOARD_PLAYER_DESTROY = 0x06
DM_BOARD_END            = 0xff

DM_RPC_STAT_RACE = 0x01
DM_RPC_STAT_CLASS = 0x02
DM_RPC_STAT_GENDER = 0x03
DM_RPC_STAT_ALIGN = 0x04
DM_RPC_STAT_HP = 0x05
DM_RPC_STAT_MAXHP = 0x06
DM_RPC_STAT_AC = 0x07
DM_RPC_STAT_POW = 0x08
DM_RPC_STAT_POWMAX = 0x09
DM_RPC_STAT_MOVES = 0x0a
DM_RPC_STAT_SCORE = 0x0b
DM_RPC_STAT_ULEVEL = 0x0c
DM_RPC_STAT_DLEVEL = 0x0d
DM_RPC_STAT_WISH = 0x0e
DM_RPC_STAT_PRAYER = 0x0f
DM_RPC_STAT_DEATHS = 0x10
DM_RPC_STAT_HAVE = 0x11
DM_RPC_STAT_DUNGEON = 0x12


# this shouldn't be global, probably
updater = None

MIN_UPDATE_PERIOD = 0.25
MAX_UPDATE_PERIOD = 60 

class BingeBoard(object):
  def __init__(self):
    self._players = []

class BingePlayer(object):
  def __init__(self, id, name):
    self._id = id
    self._name = name
    self._attrs = ["id", "name"]

    self.dispatch = {}

    # init self._name, etc, and dispatch table.
    g = globals()

    # generate list of player stats from any constant DM_RPC_STAT_*
    attrs = [x.split("_")[-1] for x in g if "DM_RPC_STAT_" in x]
    for attr in attrs:
      dispatch_key = g["DM_RPC_STAT_%s" % attr.upper()]
      #print "%s/%s: _%s = None" % (id, name, attr.lower())
      setattr(self, "_%s" % attr.lower(), None)
      if hasattr(self, "Set%s" % attr.title()):
        dispatch_method = getattr(self, "Set%s" % attr.title())
      else:
        #dispatch_method = lambda val: setattr(self, "_%s" % attr, val)
        dispatch_method = self._set_attr_method(attr)
        dispatch_method.__name__ = "_set_attr_method(%s)" % attr.lower()

      self.dispatch[dispatch_key] = dispatch_method
      self._attrs.append(attr.lower())
      del dispatch_method

  def _set_attr_method(self, attr):
    attr = attr.lower()
    return lambda value: self._set_attr(attr, value)

  def _set_attr(self, attr, value):
    real_attr = "_%s" % attr
    assert hasattr(self, real_attr), "%s has not attribute %s" % (self, real_attr)
    #print "%s: %s => %s" % (self._name, attr, value)
    setattr(self, real_attr, value)

  def update_stat(self, rpc_stat, value):
    unknown_callback = lambda val: self.unknown_stat_rpc(rpc_stat, val)
    method = self.dispatch.get(rpc_stat, unknown_callback)
    #print "%s: Update Stat: %s / %s" % (self._name, rpc_stat, method.__name__)
    method(value)

  def unknown_stat_rpc(self, rpc_stat, value):
    print "Unknown rpc stat '%s'. Stat value was '%s'" % (rpc_stat, value)
    print self.dispatch

  def clone(self):
    p = BingePlayer(self._id, self._name)
    p.__dict__.update(self.__dict__)
    return p

  def __str__(self):
    return ("%s (%s/%s/%s) moves:%s dlevel:%s exp:%s score:%s hp:%s"
            % (self._name, self._race, self._class, self._gender, 
               self._moves, self._dlevel, self._ulevel, self._score, self._hp))

  def to_dict(self):
    data = {}
    for attr in self._attrs:
      val = getattr(self, "_%s" % attr)
      if val is not None:
        data[attr] = val
    return data

class BingeBoardUpdater(threading.Thread):
  def __init__(self, binge_host):
    super(BingeBoardUpdater, self).__init__()
    self._bbc = BingeBoardConnection(binge_host)
    self._streams = []

  def run(self):
    while True:
      # Collect updates for MIN_UPDATE_PERIOD time
      start_time = time.time()
      while not self._bbc.tainted.isSet():
        self._bbc.do_one_iteration(timeout=None)

      self._bbc.tainted.clear()
      if self._streams:
        players_copy = self._bbc.get_players_copy()
        #print "Stream size: %s" % len(self._streams)
        for stream in self._streams:
          stream.update(players_copy)
      # compute diff
      # send updates to each watcher queue
      # loop over self._streams and call stream.update() ?

  def register_streamer(self, streamer):
    self._streams.append(streamer)
    streamer.update(self._bbc.get_players_copy())
    # Send full player information right now.

class BingeBoardConnection(socket.socket):
  DEFAULT_PORT = 15030

  def __init__(self, host, port=DEFAULT_BOARD_PORT):
    super(BingeBoardConnection, self).__init__(socket.AF_INET, socket.SOCK_STREAM)
    self.connect((host, port))
    self._binges = []
    self._players = {}
    self.tainted = threading.Event()
    self.tainted.clear()

    # Internal state who's stats we are being sent
    self._current_player = None

# vim to convert DM_FOO into self._rpc_foo
# yypkJi:ll2srpcllv$~Biself._A,

    self.dispatch = {
      DM_BOARD_KEEPALIVE: self._rpc_board_keepalive,
      DM_BOARD_BINGE: self._rpc_board_binge,
      DM_BOARD_PLAYER_NAME: self._rpc_board_player_name,
      DM_BOARD_STATUS: self._rpc_board_status,
      DM_BOARD_PLAYER: self._rpc_board_player,
      DM_BOARD_STAT: self._rpc_board_stat,
      DM_BOARD_PLAYER_DESTROY: self._rpc_board_player_destroy,
      DM_BOARD_END: self._rpc_board_end,
    }
    self.init_board()

  def recvall(self, byte_count):
    chunks = []
    bytes_needed = byte_count
    while bytes_needed > 0:
      data = self.recv(bytes_needed)
      bytes_needed -= len(data)
      chunks.append(data)
    return ''.join(chunks)

  def read_int(self):
    data = self.recvall(4)
    value = socket.ntohl(struct.unpack("i", data)[0])
    return value

  def write_int(self, value):
    #self.check_short(value)
    data = struct.pack("i", socket.htonl(value))
    self.sendall(data)

  def check_short(self, value):
    max = 2 << 15 - 1
    min = -(2 << 15)
    assert value >= min and value <= max, "%s is not a valid 'short'" % value

  def read_short(self):
    data = self.recvall(2)
    value = socket.ntohs(struct.unpack("h", data)[0])
    self.check_short(value)
    return value

  def write_short(self, value):
    self.check_short(value)
    data = struct.pack("h", socket.htons(value))
    self.sendall(data)

  def read_string(self):
    strlen = self.read_short()
    string = self.recvall(strlen)
    if string[-1] == "\0":
      string = string[:-1]
    else:
      print "read string was not null terminated: %r" % string
    return string

  def write_string(self, string):
    string = "%s\0" % string
    strlen = len(string)
    self.write_short(strlen)
    self.sendall(string)

  def write_byte(self, byte):
    assert byte <= 255 and byte >= 0, "byte '%s' out of range" % byte
    self.sendall(struct.pack("B", byte))

  def read_byte(self):
    data = self.recvall(1)
    byte = struct.unpack("B", data)[0]
    return byte

  def init_board(self):
    num_binges = self.read_int();
    assert num_binges > 0, "No binges found. Unexpected!"
    for i in xrange(num_binges):
      binge_name = self.read_string()
      self._binges.append(binge_name)
    self.choose_binge(self._binges[0])

  def choose_binge(self, binge_name):
    self.write_byte(DM_BOARD_BINGE)
    self.write_string(binge_name)

  def do_one_iteration(self, timeout=None):
    (input, unused, exc) = select.select([self], [], [self], timeout)

    # If there continues to be data, we have a small time to read do as
    # much as we can
    start_time = time.time()
    extended_time = .1
    iteration_count = 0
    while input:
      iteration_count += 1
      if input:
        cmd = self.read_byte()
        unknown_callback = lambda : self._unknown_rpc(cmd)
        method = self.dispatch.get(cmd, unknown_callback)
        method()
        self.tainted.set()
      if exc:
        print "Socket error. Probably should clean up and die."
      time_left = extended_time - (time.time() - start_time)
      if time_left <= 0:
        break
      (input, unused, exc) = select.select([self], [], [self], time_left)

    #print "Read %d entries that time" % iteration_count

  def _unknown_rpc(self, cmd):
    print "Unknown rpc '%d' received" % cmd

  def _rpc_board_keepalive(self):
    pass

  def _rpc_board_binge(self):
    print "Received OK for selected binge"

  def _rpc_board_player_name(self):
    id = self.read_int()
    name = self.read_string()
    self._current_player = id
    print "player: %s / %s" % (id, name)
    if id not in self._players:
      self._players[id] = BingePlayer(id, name)

  def _rpc_board_status(self):
    status = self.read_int()
    print "status: %d" % status

  def _rpc_board_player(self):
    id = self.read_int()
    #print "board player: %d" % id
    self._current_player = id

  def _rpc_board_stat(self):
    assert self._current_player is not None, "BOARD_STAT seen before first PLAYER received"
    assert self._current_player in self._players, "Player '%d' not known" % self._current_player

    stat_key = self.read_byte()
    if stat_key in (DM_RPC_STAT_RACE, DM_RPC_STAT_CLASS,
                    DM_RPC_STAT_GENDER, DM_RPC_STAT_DUNGEON):
      value = self.read_string()
    elif stat_key == DM_RPC_STAT_HAVE:
      value = self.read_byte()
    else:
      value = self.read_int()

    self._players[self._current_player].update_stat(stat_key, value)
    # Add this change to each watcher's queue

  def _rpc_board_player_destroy(self):
    id = self.read_int()
    del self._players[id]

  def _rpc_board_end(self):
    print "Destroy RPC received"

  def get_players_copy(self):
    players_copy = {}
    for id, player in self._players.iteritems():
      players_copy[id] = player.clone()
    return players_copy

class BingeCometServer(threading.Thread):
  def __init__(self, port=8080):
    super(BingeCometServer, self).__init__()
    self._port = port

  def run(self):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(("0.0.0.0", self._port))
    server.listen(5)
    inputs = [server]
    outputs = []
    clients = []

    while True:
      in_ready, out_ready, exc_ready = select.select(inputs, outputs, [])
      for s in in_ready:
        if s == server:
          (client_socket, client_addr) = s.accept()
          print "New client: %s" % (client_addr,)
          client_thread = BingeCometStreamClient(client_socket)
          client_thread.start()

class ThreadingHTTPServer(SocketServer.ThreadingMixIn,
                           BaseHTTPServer.HTTPServer):
  pass

class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  content_types = {
    "css": "text/css",
    "html": "text/html",
    "png": "image/png",
    "jpg": "image/jpeg",
    "gif": "image/gif",
    "js": "text/plain",
  }
  def do_GET(self):
    streamer = BingeCometStreamer(self)
    if "?" in self.path:
      (path, query) = self.path.split("?", 1)
    else:

      (path, query) = (self.path, "")
    if path == "/":
      path = "/index.html"

    if path == "/bingerpc":
      fields = cgi.parse_qs(query)
      callback = fields.get("callback", [None])[0]
      streamer.stream(callback)
    else:
      self.serve_file(path)

  def serve_file(self, path):
    fullpath = os.path.abspath(path[1:])
    pwd = os.getcwd()
    if not fullpath.startswith(pwd):
      self.serve_error(403, "Invalid path: %s" % path)
      return

    if os.path.isdir(fullpath):
      self.serve_error(403, "Directory listing not implemented")
      return

    try:
      fd = open(fullpath, "r")
    except IOError, e:
      self.serve_error(403, "Error occured while opening file.")
      print >>sys.stderr, e
      return

    extension = fullpath.split(".")[-1]
    content_type = self.content_types.get(extension, "text/plain")

    self.send_response(200)
    self.send_header("Content-type", content_type)
    self.end_headers()
    for i in fd:
      self.wfile.write(i)
    fd.close()

  def serve_error(self, code, message):
    self.send_response(code)
    self.send_header("Content-type", "text/plain")
    self.end_headers()

    self.wfile.write(message)

class BingeCometStreamer(object):
  def __init__(self, request_handler_object):
    self._response = request_handler_object
    self._players = None
    self._lock = threading.Condition()

  def update(self, players):
    #print "Updating from %s" % threading.currentThread()
    self._lock.acquire()
    self._players = players
    self._lock.notify()
    self._lock.release()
    #print "release playerlist: %s" % threading.currentThread()

  def stream(self, callback):
    delim = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    self._response.send_response(200)
    self._response.send_header("Content-type",
                     "multipart/x-mixed-replace;boundary=%s" % delim)
    self._response.end_headers()
    out = lambda data: self._response.wfile.write(data)

    updater.register_streamer(self)

    old_players = {}
    while True:
      start_time = time.time()
      #print "lock playerlist: %s" % threading.currentThread()
      self._lock.acquire()
      while self._players is None:
        #print "wait playerlist: %s" % threading.currentThread()
        self._lock.wait()
      players = self._players
      self._players = None
      #print "release playerlist: %s" % threading.currentThread()
      self._lock.release()

      lines = []
      lines.append("--%s" % delim)
      lines.append("Content-type: text/plain")
      lines.append("")

      # Compute diff of old_players and players
      updated_players = self.diff_players(old_players, players)
      player_list = [x.to_dict() for x in updated_players.itervalues()]
      lines.append("%s(" % callback)
      lines.append(simplejson.dumps(player_list))
      lines.append(");")
      lines.append("--%s" % delim)
      try:
        out("\r\n".join(lines))
      except:
        return

      old_players = players
      sleep_time = .25 - (time.time() - start_time)
      if sleep_time < 0:
        sleep_time = 0
      time.sleep(sleep_time)

  def diff_players(self, old_players, new_players):
    delta_players = {}
    if not old_players:
      print "Old players is empty.. skipping diff"
      return new_players

    for old_id in old_players:
      if old_id not in new_players:
        deleted.append[old_id]
        continue
      old = old_players[old_id]
      new = new_players[old_id]
      delta = BingePlayer(old_id, old._name)
      diff = False
      for attr in ["_%s" % x for x in old._attrs]:
        old_val = getattr(old, attr)
        new_val = getattr(new, attr)
        #print "%s(%s): %s vs %s" % (new._name, attr, old_val, new_val)
        if old_val != new_val:
          diff = True
          setattr(delta, attr, new_val)
      if diff:
        delta_players[delta._id] = delta
    return delta_players



#cometserver = BingeCometServer(int(sys.argv[1]))
#cometserver.start()
updater = BingeBoardUpdater("nethacks.org")
updater.start()
server = ThreadingHTTPServer(("0.0.0.0", 8080), HTTPHandler)
server.serve_forever()

#bbc = BingeBoardConnection("nethacks.org")
#while True:
  #bbc.do_one_iteration()
