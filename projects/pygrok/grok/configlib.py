
class ConfigParser(object):
  def __init__(self, filename):
    self.filename = filename

  def parse(self):
    self.fd = open(self.filename)
    self.fd.close()

if __name__=="__main__":
  c = ConfigParser("/dev/stdin")
  c.parse()
