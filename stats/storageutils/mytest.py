

import unittest

class EnhancedTestCase(unittest.TestCase):
  def assertListEquals(self, list_a, list_b):
    self.assertEquals(len(list_a), len(list_b))
    index = -1
    for (a, b) in zip(list_a, list_b):
      index += 1
      if a == b:
        continue
      errmsg = "%s != %s at index %d. (%r != %r)" % (list_a, list_b, index, a, b)
      raise AssertionError(errmsg)

main = unittest.main
