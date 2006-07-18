import debuglib
import re

class ConfigParser(object):
  whitespace = re.compile(r'\s+')
  reactionStart = re.compile(r'reactions\s*{')
  reactionEntry = re.compile(r'(?P<name>\w+)\s*=\s*(?P<command>.*)')
  reactionEntryDelimiter = re.compile(r'\n')
  reactionEnd = re.compile(r'}')

  def __init__(self, data=None, file=None):
    self.data = data
    self.file = file

  def readfile(self):
    try:
      self.fd = open(self.file, "r")
      self.data = self.read
      self.fd.close()
    except Exception, e:
      debuglib.fatal(e)
      raise SystemExit(e)

  def parse(self):
    if self.file:
      self.readfile()
    self._startparse()

  def _startparse(self):
    self._buffer = self.data
    while len(self._buffer) > 0:
      print "B: %s" % self._buffer
      self.consumeWhitespace()
      #self.consumeComment()
      self.parseReaction()
      #self.parseFilter()
      #self.parsePattern()
      #self.parseRule()

  def consume(self, m):
    if m:
      self._buffer = self._buffer[m.end():]

  def consumeWhitespace(self):
    self.consume(self.whitespace.match(self._buffer))

  def parseComment(self):
    reg_comment = re.compile(r"#|/\*|//");
    # self.consumeComment()

  def parseReaction(self):
    #print self.reactionStart.pattern
    start = self.reactionStart.match(self._buffer)
    if start:
      self.consume(start)

    #reactionStart = r'reaction\s*{'
    #reactionEntry = r'(?P<name>\w+)\s*=\s*(?P<command>.*)'
    #reactionEntryDelimiter = r'\n'
    #reactionEnd = r'}'

  def parseFilter(self):
    pass

  def parsePattern(self):
    pass

  def parseRule(self):
    pass

testconf="""
  reactions {
    one = bar
    two = fizz
  }
"""

if __name__=="__main__":
  debuglib.level = debuglib.INFO
  c = ConfigParser(data=testconf)
  c.parse()
