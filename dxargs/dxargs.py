#!/usr/bin/env python

import sys
import os
import re

import threading
import time
import popen2
from Queue import Queue

from optparse import OptionParser
import logging

parser = OptionParser()
parser.add_option("-n", type="int", dest="n", default=500)
parser.add_option("-P", type="int", dest="P", default=1)
parser.add_option("-l", type="int", dest="l")
parser.add_option("-t", action="store_true", dest="t")
parser.add_option("--verbose", action="store_true", dest="verbose")
parser.add_option("--output_dir", dest="output_dir")
parser.add_option("--report_output_filenames", action="store_true",
                  dest="report_output_filenames")
parser.add_option("--hosts_file", dest="hosts_file")
parser.add_option("--hosts", dest="hosts")
parser.add_option("--ssh_sock_dir", dest="ssh_sock_dir", default="/tmp")
parser.add_option("--ssh_opts", dest="ssh_opts", 
                  default="-o 'StrictHostKeyChecking no' -c blowfish -C -T")

sessions = {}

output_locks = {
  sys.stdout: threading.Lock(),
  sys.stderr: threading.Lock(),
}

# This gets set if you use --output_dir
input_set_loghandle = None

class ThreadPool(object):
  def __init__(self, size):
    self.size = size
    self.queue = Queue()
    self.done = threading.Event()
    self.done.clear()

    self.workers = []
    for i in range(size):
      worker = self.SSHWorker(self.queue, self.done)
      self.workers.append(worker)

  def addtask(self, command, args, task_id):
    self.queue.put(self.Task(command, args, task_id))

  def start(self):
    for i in self.workers:
      i.start()

  def finish(self):
    def die():
      raise SystemExit()

    for i in range(self.size):
      self.addtask(die, self, None)
    for i in range(self.size):
      logging.info('Waiting on thread %d' % i)
      self.workers[i].join()

  class Task(object):
    def __init__(self, command, args=[], task_id=-1):
      self.command = command
      self.args = args
      self.task_id = task_id

  class SSHWorker(threading.Thread):
    sessions = {}

    def __init__(self, queue, done):
      self.queue = queue
      self.done = done
      self._pid = os.getpid()
      threading.Thread.__init__(self)

    def SetParams(self, host, index):
      logging.info("Setting worker params: %s/%s" % (index, host))
      self._host = host
      self._index = index
      self._proc = None
      self._path = "%s/dxargs.%s.%d" % (options.ssh_sock_dir, host, index)
      self._ssh_cmd = ("ssh -nNM -S '%s' %s %s" 
                       % (self._path, options.ssh_opts, self._host))

    def run(self):
      for i in ("host", "index", "path"):
        if not hasattr(self, "_%s" % i):
          fatal("required attr '%s' not set for this task" % i)

      try:
        self.real_run()
      finally:
        if os.path.exists(self._path):
          os.unlink(self._path)

    def real_run(self):
      if os.path.exists(self._path):
        # fatal runs 'cleanup' so don't use fatal here.

        print >>sys.stderr, ("ssh control socket already exists, "
                             "cannot continue: %s" % self._path)
        sys.exit(1)

      functype = lambda : True
      while not self.done.isSet():
        task = self.queue.get()
        if hasattr(task.command, "__call__"):
          task.command()
        else:
          if self._proc is None:
            self.get_session()
          self.handle_task(task)

    def get_session(self):
      if self._ssh_cmd in self.sessions:
        logging.info("Starting master session for %d/%s" 
                     % (self._index, self._host))
        self._proc = self.sessions[self._ssh_cmd]
      else:
        logging.info("Using existing master session to %d/%s" 
                     % (self._index, self._host))
        self._proc = popen2.Popen3(self._ssh_cmd)

    def handle_task(self, task):
      #global options
      command = " ".join(task.command)
      args = " ".join(task.args)
      proc = Proc(self._path)
      #proc.start()

      if options.t:
        output(sys.stderr, "%d/%s: %s %s\n" 
               % (self._index, self._host, command, task.args,))

      proc.runcmd("set -- %s" % args)
      proc.runcmd("JOBINDEX=%d" % (self._index))
      proc.runcmd("TASK=%d" % (task.task_id))
      proc.runcmd(command)
      proc.runcmd("exit")

      fd = sys.stdout
      if options.output_dir:
        # filename == DIR/PID.TASKID.INDEX.HOST.TIME
        filename = ("%s/%d.%d.%d.%s.%d" %
                    (options.output_dir,
                     self._pid,
                     task.task_id,
                     self._index,
                     self._host, 
                     time.time()))
        fd = open(filename, "w")
        output(input_set_loghandle, "%s %s\n" % (filename, args))

      return_code = -1
      while True:
        line = proc.readline()
        if not line:
          return_code = proc.finish()
          break
          #logging.error("Unexpected EOF from proc on %s" % host)
        output(fd, line)

      fd.close()
      output(sys.stdout, "%s\n" % filename)

      if return_code != 0:
        fatal("Nonzero return code from child on host '%s'.\nCommand was: %s" 
              % (self._host, command))

class Proc(object):
  def __init__(self, ssh_ctl_path):
    command = "/bin/sh"
    self._ssh_cmd = ("ssh %s -S '%s' dummy_hostname %s" 
                     % (options.ssh_opts, ssh_ctl_path, command))
    self._ctl_path = ssh_ctl_path
    logging.info("Creating slave session for %s" % (ssh_ctl_path))

    # Lazy instantiation
    self._proc = None

  def start(self):
    trycount = 0
    while trycount < 10:
      if os.path.exists(self._ctl_path):
        self._proc = popen2.Popen3(self._ssh_cmd)
        break
      logging.info("Waiting for control socket to exist: %s" % self._ctl_path)
      trycount += 1
      time.sleep(1)

    if not self._proc:
      fatal("Timeout waiting for master to create control socket: %s" 
            % self._ctl_path)

  def runcmd(self, cmd):
    if not self._proc:
      self.start()
    self._proc.tochild.write("%s\n" % cmd)
    self._proc.tochild.flush()

  def finish(self):
    self._proc.tochild.close()
    self._proc.fromchild.close()
    return self._proc.wait()

  def readline(self):
    return self._proc.fromchild.readline()

def fatal(msg):
  print >>sys.stderr, "Fatal: %s" % msg
  sys.exit(1)

def output(fd, data):
  if fd in output_locks:
    output_locks[fd].acquire()

  fd.write(data)

  if fd in output_locks:
    fd.flush()
    output_locks[fd].release()

def expand_hosts(hostlist):
  new_hostlist = []
  for i in hostlist:
    if "*" in i:
      (host, repeats) = i.split("*", 1)
      try:
        repeats = int(repeats)
      except Exception, e:
        fatal("Failed to convert '%s' to integer in string '%s'" % (repeats, i))
      new_hostlist.extend([host for j in range(repeats)])
    else:
      new_hostlist.append(i)

  return new_hostlist

def main():
  global options
  global input_set_loghandle

  (options, args) = parser.parse_args()
  logging.basicConfig(
    level=(options.verbose and logging.INFO or logging.WARNING),
    format="%(asctime)s %(levelname)s %(message)s"
  )

  if options.output_dir:
    filename = "%s/%d.input_map" % (options.output_dir, os.getpid())
    input_set_loghandle = open(filename, "w")
    output_locks[input_set_loghandle] = threading.Lock()

  if not os.path.isdir(options.ssh_sock_dir):
    fatal("ssh socket dir is not a directory or does not exist: %s" 
             % options.ssh_sock_dir)

  if not os.access(options.ssh_sock_dir, os.W_OK | os.X_OK):
    fatal("ssh socket dir is not writable: %s" % options.ssh_sock_dir)

  if options.hosts:
    hosts = options.hosts.split()
  elif options.hosts_file:
    fd = open(options.hosts_file, "r")
    hosts = [x.strip() for x in fd.readlines() if not x.startswith("#")]
    fd.close()
  else:
    fatal("No hosts to connect to. Specirfy --hosts or --hosts_file")

  hosts = expand_hosts(hosts)

  if options.P == 0:
    options.P = len(hosts)

  if len(hosts) < options.P:
    fatal("Parallelism exceeds size of host list. Host list size must be"
             "larger than parallelism")

  if len(args) == 0:
    command = ["echo", '"$@"']
  else:
    command = args

  tokens = []
  host_index = 0;
  pool = ThreadPool(options.P)

  tmp = 0
  for i in pool.workers:
    i.SetParams(hosts[tmp], tmp)
    tmp += 1

  pool.start()

  # XXX: Should we loop to hosts[options.P] or always rotate through
  # XXX: Should we read bytes here, instead of lines?

  task_id = 0
  for i in sys.stdin:
    cur_tokens = [x for x in i.split()]
    tokens.extend(cur_tokens)
    while len(tokens) >= options.n:
      pool.addtask(command, tokens[:options.n], task_id)
      del tokens[:options.n]
      host_index += 1
      task_id += 1

      #if host_index == len(hosts):
      if host_index == options.P:
        host_index = 0

  # At this point, we might have some input remaining to process
  if len(tokens) > 0:
    pool.addtask(command, tokens[:options.n], task_id)
    del tokens[:]

  pool.finish()

if __name__ == "__main__":
  main()
