
def profile(func, *args):
  import hotshot
  output = "/tmp/my.profile"
  p = hotshot.Profile(output)
  p.runcall(func, *args)
  p.close()

def f(d):
  return d.keys()

x=dict()
for i in range(1000000):
  x[i] = i

profile(f, x)
