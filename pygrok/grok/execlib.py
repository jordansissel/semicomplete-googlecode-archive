import os

class System(object):
  shellproc = None
  use_subshell = True
  use_standalone_shell = False

  def run(self,cmd):
    if self.shellproc is None:
      self.shellproc = os.popen("/bin/sh", "w")
    if self.use_subshell:
      cmd = "( %s )" % cmd
    self.shellproc.write("%s\n" % cmd)
    self.shellproc.flush()

    if self.use_standalone_shell:
      self.shellproc.close()
      self.shellproc = None

  run = classmethod(run)
