import time, sys

for x in sys.stdin:
  x = x.strip()
  arr = x.split(None, 1)
  if len(arr) != 2:
    print "Bad; %s" % x
    continue
  (x, y) = arr
  timestamp = ((time.mktime(time.strptime(x, "%d/%b/%Y:%H:%M:%S")) + (60*60*4)) * 1000000)

  clients = ("google", "yahoo", "msn", "twiceler")
  client = "human"
  for i in clients:
    if i in y.lower():
      client = i
  print "agent.%s@%d" % (client, timestamp)

