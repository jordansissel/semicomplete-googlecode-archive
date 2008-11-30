TEST=$1

if [ -z "$TEST" ] ; then
  echo "Usage: $0 <test>"
  exit 1
fi

echo "=> Building $TEST"
cout=`make clean $TEST 2>&1`
if [ $? -ne 0 ]; then
  echo "$cout"
  echo "Compile for $TEST failed."
else
  ./$TEST | egrep '^  (Test:|  [0-9])'
fi


