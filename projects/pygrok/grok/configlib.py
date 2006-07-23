import re
from pprint import pprint
import debuglib

class ExpectedTokenMissing(Exception):
  pass

class Tokenizer(object):
  def __init__(self):
    self.re = re.compile(self.regex)

  def match(self, data):
    return self.re.match(data)

  def consume(self, data):
    ret = {}
    m = self.re.match(data)
    if m is None:
      raise ExpectedTokenMissing("Expected: %s" % self.__class__.__name__)
    return m.group(), m.string[m.end():]

  def getPattern(self):
    return self.re.pattern

class TWhitespace(Tokenizer):
  regex = r'\s+'

class TWord(Tokenizer):
  regex = r'\w+'

class TOpenBlock(Tokenizer):
  regex = r'{'

class TCloseBlock(Tokenizer):
  regex = r'}'

class TComment(Tokenizer):
  regex = r'#.*'

class TEquals(Tokenizer):
  regex = r'='

class TString(Tokenizer):
  regex = "|".join((
    r"\w+", # word
    r'"""(?:.*?)"""', # python triple-double quote
    r"'''(?:.*?)'''", # python triple-single quote
    r'"(?:[^\\"]*(?:\\.[^\\"]*)*)"',  # double quoted
    r"'(?:[^\\']*(?:\\.[^\\']*)*)'",  # single quote
    ))

class ConfigParser(object):
  Word = TWord()
  Whitespace = TWhitespace()
  Comment = TComment()
  OpenBlock = TOpenBlock()
  CloseBlock = TCloseBlock()
  Equals = TEquals()
  String = TString()

  def __init__(self, filename=None, data=None):
    if filename:
      self.data = self.readfile(filename);
    else:
      self.data = data

    self.lineno = 1
    self.config = {}

    self.blocktypes = { 
    #( "reactions", "filters", "patterns", "file", "exec", "rule")
      "reactions": self.parseBlock,
      "filters": self.parseBlock,
      "patterns": self.parseBlock,
      "file": self.parseFileBlock,
      "exec": self.parseFileBlock,
      "rule": self.parseRuleBlock,
    }

  def readfile(self, filename):
    try:
      fd = open(filename)
      data = "".join(fd.readlines())
      fd.close()
    except Exception(e):
      print e

    return data

  def parse(self):
    try:
      self.buf = self.data
      # Cut out comments:
      self.eatignorable()

      # attempt to find 'foo {' or 'foo ='
      while len(self.buf) > 0:
        word = None
        print "l: %d" % len(self.buf)
        if not self.Word.match(self.buf):
          debuglib.fatal("Unknown: %s" % self.buf)
        word, self.buf = self.Word.consume(self.buf)
        self.eatignorable()
        print "Word: %s" % word
        if word in self.blocktypes:
          if not word in self.config:
            self.config[word] = {}
          self.config[word].update(self.blocktypes[word]())
        else:
          keyname = 'globals'
          if not keyname in self.config:
            self.config[keyname] = {}
          self.config[keyname].update(self.parseAssignment(key=word))

        self.eatignorable()

      print "Config: "
      pprint(self.config)

    except ExpectedTokenMissing, e:
      debuglib.fatal("config line %d: %s" % (self.lineno, e))
      raise SystemExit

  def parseBlock(self):
    ignored, self.buf = self.OpenBlock.consume(self.buf)
    data = {}
    self.eatignorable()
    try:
      while not self.CloseBlock.match(self.buf):
        data.update(self.parseAssignment())
      ignored, self.buf = self.CloseBlock.consume(self.buf)
    except ExpectedTokenMissing, e:
      debuglib.fatal("config line %d: %s" % (self.lineno, e))
    return data

  def parseFileBlock(self):
    file, self.buf = self.String.consume(self.buf)
    data = {}
    data['_filename'] = file
    self.eatignorable()
    data.update(self.parseBlock())
    return data

  def parseRuleBlock(self):
    return {}

  def parseAssignment(self, key=None):
    self.eatignorable()
    if key is None:
      key, self.buf = self.Word.consume(self.buf)
    self.eatignorable()
    ignored, self.buf = self.Equals.consume(self.buf)
    self.eatignorable()
    value, self.buf = self.String.consume(self.buf)
    self.eatignorable()

    #return key, value
    return { key: value }

  def parseFilters(self):
    print "Fil"

  def parsePatterns(self):
    print "Patt"

  def saveBlock(self, word, data):
    if not self.config.has_key(word):
      self.config[word] = {}
    self.config[word].update(data);

  def eatignorable(self):
    """ Eat comments and whitespace, modifies internal buffer """
    match = True
    while match:
      if self.Whitespace.match(self.buf):
        match = True
        tok, self.buf = self.Whitespace.consume(self.buf)
        eol = re.finditer("\n", tok)
        for i in eol:
          self.lineno += 1
      else:
        match = False
      if self.Comment.match(self.buf):
        match = True
        tok, self.buf = self.Comment.consume(self.buf)
      else:
        match = False

if __name__ == "__main__":
  c = ConfigParser("grok.conf")
  c.parse()
  #x = TString()
  #print x.match(r'"Foo bar baz"').group()
  #print x.match(r"'Foo b\'arbaz'").group()
  #print x.match(r"'''Foo b' 'ar baz'''").group()
  ##print x.match(r'"""Foo b" ""ar baz"""').group()
  #print x.match(r'foobar').group()

