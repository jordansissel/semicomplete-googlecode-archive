

class Cache(dict):
  """ Simple limited cache.

  Behaves likes a dict object, except it expires items in FIFO order.
  Expiration of old items happens upon insertion.
  """

  def __init__(self, size=100):
    """ Init.

    Args:
      size: maximum number of items allowed in the cache
    """
    self._maxsize = size
    self._key_list = []

  def __setitem__(self, key, value):
    if not self.has_key(key):
      self.__add_key(key)
    return dict.__setitem__(self, key, value)

  def __delitem__(self, key):
    if key in self._key_list:
      self._key_list.remove(key)
    return dict.__delitem__(self, key)

  def __add_key(self, key):
    self._key_list.append(key)
    if len(self._key_list) > self._maxsize:
      del self[self._key_list.pop(0)]

def memoize(func):
  """ Memoize a function or method.

  This works on class methods in addition to functions.
  """
  m = Memoize(func)
  def newfunc(*args, **kwds):
    return m(*args, **kwds)
  newfunc.__name__ = "(memoized)%s" % func.__name__
  newfunc.__doc__ = func.__doc__
  return newfunc

class Memoize(object):
  """ Memoizing decorator class.

  Taken mostly from:
  http://wiki.python.org/moin/PythonDecoratorLibrary
  """
  def __init__(self, func, size=100):
    self._func = func
    self._cache = Cache(size)
    self.__name__ = "(memoized)%s" % func.__name__

  def __call__(self, *args, **kwds):
    try:
      return self._cache[args]
    except KeyError:
      self._cache[args] = value = self._func(*args, **kwds)
      return value
    except TypeError:
      return self._func(*args)

  def __repr__(self):
    return self._func.__doc__
