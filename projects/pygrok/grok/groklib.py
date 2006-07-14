import sys
import os
import re
#from grok.debuglib import debug, DEBUG, INFO, WARN, FATAL
import execlib
from debuglib import info, warn, fatal, debug

GLOBALPM = {
  'HOST': '[\w+\._-]+',
  'PORT': '\d+',
  'PROG': '\w+',
  'USER': '\w+',
}

# XXX: Class me?
tokenregex = re.compile("(?P<substr>%\((?P<fullname>(?P<patname>[A-Za-z0-9]+)(?:_(?P<subname>[A-Za-z0-9]+))?)\))")

class Rule(object):
  def __init__(self, reaction, patterns, options={}):
    info("New rule created:")
    info("cmd: %s" % reaction)
    self.reaction = Reaction(reaction, options)
    self.patterns = [ Pattern(p, options) for p in patterns ]
    self.options = options

  def evaluate(self, data):
    for pat in self.patterns:
      m = pat.apply(data)
      if m:
        self.react(m, data)

  def react(self, keywords, data):
    self.reaction.execute(keywords, data)

class Pattern(object):
  def __init__(self, pattern, options={}):
    try:
      self.patternMap = options['pattern map']
    except KeyError, e:
      fatal("No pattern map specified when creating Pattern")
      raise SystemExit
    self.pattern = pattern
    self.generateRegex()

  def generateRegex(self):
    regexString = self.pattern
    self.matches = tokenregex.finditer(self.pattern)
    self.matchlist = [i for i in self.matches]
    for m in self.matchlist:
      md = m.groupdict()
      if self.patternMap.has_key(md['patname']):
        repl = self.generatePatternRegex(md['patname'], md['fullname'])
        debug("Replacing %s with %s"  % (md['substr'], repl))
        regexString = regexString.replace(md['substr'],repl)
    self.saveRegex(regexString)

  def generatePatternRegex(self, patname, fullname):
    return "(?P<%s>%s)" % (fullname, self.patternMap[patname])

  def saveRegex(self, regexString):
    info("Saving regex; %s" % regexString)
    self.regexString = regexString
    self.regex = re.compile(regexString)

  def apply(self, data):
    m = self.regex.search(data)
    if m:
      return m.groupdict()
    return None

class Reaction(object):
  def __init__(self, command, options):
    self.command = command
    self.options = options

  def execute(self, keywords, data):
    cmd = self.command
    matches = tokenregex.finditer(cmd)
    matchlist = [i for i in matches]
    for m in matchlist:
      md = m.groupdict()
      if keywords.has_key(md['fullname']):
        repl = keywords[md['fullname']]
        cmd = cmd.replace(md['substr'], repl)
    #execlib.System.use_subshell = False
    #execlib.System.use_standalone_shell = True
    execlib.System.run(cmd)

if __name__ == "__main__":
  line = '%(HOST) %(PROG)\[\d+\]: error: PAM: authentication error for %(USER) from %(HOST_SRC)'
  pm = {
    'HOST': '[\w+\._-]+',
    'PORT': '\d+',
    'PROG': '\w+',
    'USER': '\w+',
  }
  rules = [
    Rule(

      "echo '%(HOST)/%(PROG): login failure for %(USER) (src %(HOST_SRC))'",

      [ 
        '%(HOST) %(PROG)\[\d+\]: error: PAM: authentication error for %(USER) from %(HOST_SRC)',
        '%(HOST) %(PROG)\[\d+\]: Invalid user %(USER) from %(HOST_SRC)',
      ],

      {
        'pattern map': GLOBALPM,
      },

    )
  ] 

  while 1:
    l = sys.stdin.readline()
    if not l:
      fatal("stdin closed")
      raise SystemExit
    for rule in rules:
      rule.evaluate(l)
