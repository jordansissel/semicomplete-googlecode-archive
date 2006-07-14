import sys,os

# enum-type hack, hehe.
(DEBUG,INFO,WARN,FATAL,NONE) = range(1,6)

"""
XXX: The sys._getframe stuff seems kind of sketchy to me. Oh well, knowing the
caller is good.
"""

level=NONE

def info(message):
  log(INFO, message)

def warn(message):
  log(WARN, message)

def debug(message):
  log(DEBUG, message)

def fatal(message):
  log(FATAL, message)

def log(type, message):
  if type < level: 
    return
  frame = sys._getframe(2)
  msg = "%(file)s:%(lineno)d] %(class)s.%(caller)s: %(message)s" % {
    'class': _getclass(frame.f_locals, 'self'),
    'file': _getfile(frame.f_code.co_filename),
    'lineno': frame.f_lineno,
    'module': frame.f_globals['__name__'], # __main__ if nowhere
    'caller': frame.f_code.co_name, # Set to '?' if in main scope (no method/class)
    'message': message,
  }

  #print msg
  sys.stderr.write("%s\n" % msg)

def _getclass(vars, key):
  if vars.has_key('self'): 
    return vars['self'].__class__.__name__
  return '???'

def _getfile(path):
  return os.sep.join(path.split(os.sep)[-2:])

info("Loaded: %s" % __name__)
