import debuglib

class ConfigParser(object):
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

testconf="""
  reactions {
    foo: "bar",
    baz: "fizz",
  }
"""

if __name__=="__main__":
  c = ConfigParser(data=testconf)
  c.parse()
