import debuglib
from groklib import Rule, Reaction, PyReaction, Pattern
import re

class ConfigParser(object):

  stack = []
  def pushState(st):
    ConfigParser.stack.append({ '_state': st })

  def popState():
    return ConfigParser.stack.pop()

  def storekv(k,v):
    print "%s: %s" % (k,v)

  configPatterns = {
    'WORD': '\w+',
    'REACTION': '.*$',
  }

  userrules = []

  parseglobals = {
    'pushState': pushState,
    'popState': popState,
    'storekv': storekv,
  }

  shortrules = {
    r'reactions {': 'pushState("reactions")',
    '%(WORD)\s*=\s*%(REACTION)': 'storekv("%(WORD)", "%(REACTION)")',
    '}': 'popState()',
  }

  rules = []
  ruleglobals = {
    'pushState': pushState,
    'popState': popState,
  }

  ruleoptions = { 'pattern map': configPatterns, 'globals': ruleglobals }
  for r in shortrules:
    reaction = PyReaction(shortrules[r], options=ruleoptions)
    patterns = r;
    rules.append(Rule(reaction=reaction, patterns=[r], options=ruleoptions))

  def __init__(self, filename=None, data=None):
    self.filename = filename
    self.data = data

  def parse(self):
    if self.filename:
      try:
        fd = file(self.filename)
        self.data = fd.read()
        fd.close()
      except Exception, e:
        debuglib.fatal(e)
        raise SystemExit(e)

    lines = self.data.split("\n");
    for l in lines:
      #print "Trying: '%s'" % l
      for r in self.rules:
        r.evaluate(l)

if __name__ == "__main__":

  p = ConfigParser(data="""
  reactions {
    foo = bar
    baz = fizz
  }
  """)

  p.parse()
