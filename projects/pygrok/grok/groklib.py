import sys
import os
import re

import execlib
import debuglib

# This should live somewhere, perhaps passed in to Rule creations
GLOBALPM = {
  'HOST': '[\w+\._-]+',
  'PORT': '\d+',
  'PROG': '\w+',
  'USER': '\w+',
}

# XXX: Where should this live?
tokenregex = re.compile("(?P<substr>%\((?P<fullname>(?P<patname>(?:_)?[A-Za-z0-9]+)(?:_(?P<subname>[A-Za-z0-9]+))?)\))")

class Rule(object):
  default_options = {
    'interval': 0,
    'hits_per_interval': 0
  }

  def __init__(self, reaction, patterns, options={}):
    debuglib.info("New rule created:")
    self.reaction = Reaction(reaction, options)
    self.patterns = [ Pattern(p, options) for p in patterns ]
    self.options = Rule.default_options
    # XXX: dict.update() merges dicts.
    self.options.update(options)

    self.hits = 0

  def evaluate(self, data):
    """ Evaluate data against this rule's patterns. Calls 'hit' if match succeds. """
    for pat in self.patterns:
      m = pat.apply(data)
      if m:
        self.hit(m, data)

  def hit(self, keywords, data):
    """ Register a hit for this particular rule. React if necessary """
    self.hits += 1
    keywords.update({
      '_HITS': self.hits
    })

    if self.options['interval']:
      # Check interval
      pass
    else:
      self.react(keywords, data)

  def react(self, keywords, data):
    self.reaction.execute(keywords, data)

class Pattern(object):
  def __init__(self, pattern, options={}):
    try:
      self.patternMap = options['pattern map']
    except KeyError, e:
      debuglib.fatal("No pattern map specified when creating Pattern")
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
        debuglib.debug("Replacing %s with %s"  % (md['substr'], repl))
        regexString = regexString.replace(md['substr'],repl)
    self.saveRegex(regexString)

  def generatePatternRegex(self, patname, fullname):
    return "(?P<%s>%s)" % (fullname, self.patternMap[patname])

  def saveRegex(self, regexString):
    debuglib.info("Saving regex; %s" % regexString)
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
        # XXX: repl must be a string.
        repl = "%s" % keywords[md['fullname']]
        cmd = cmd.replace(md['substr'], repl)
    execlib.System.run(cmd)

if __name__ == "__main__":
  debuglib.level = debuglib.INFO

  rules = [
    Rule(
      reaction = "echo 'authfail: %(_HITS): %(HOST)/%(PROG): login failure for %(USER) (src %(HOST_SRC))'",
      patterns = [ 
        '%(HOST) %(PROG)\[\d+\]: error: PAM: authentication error for %(USER) from %(HOST_SRC)',
        '%(HOST) %(PROG)\[\d+\]: Invalid user %(USER) from %(HOST_SRC)',
      ],
      options = {
        'pattern map': GLOBALPM,
      },

    ),
    Rule(
      reaction = "echo 'noidstring: %(_HITS): %(HOST)/%(PROG): src %(HOST_SRC)'",
      patterns = [ 
        '%(HOST) %(PROG)\[\d+\]: Did not receive identification string from %(HOST_SRC)',
      ],
      options = {
        'pattern map': GLOBALPM,
      },
    ),
  ]

  while 1:
    l = sys.stdin.readline()
    if not l:
      debuglib.fatal("stdin closed")
      raise SystemExit
    for rule in rules:
      rule.evaluate(l)
