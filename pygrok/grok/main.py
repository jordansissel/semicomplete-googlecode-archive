import sys
import re
from groklib import LineGrokker, RegexGenerator
from debuglib import debug, DEBUG, INFO, WARN, FATAL

if __name__ == "__main__":
  g = LineGrokker([
    '%(HOST) %(PROG)\[\d+\]: error: PAM: authentication error for %(USER) from %(HOST_SRC)',
    ])
  while 1:
    l = sys.stdin.readline()
    g.grok(l)
