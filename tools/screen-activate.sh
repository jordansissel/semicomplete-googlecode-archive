#!/bin/sh

if [ $# -ne 1 ] ; then
  echo "usage: $0 <screen_session>"
  exit 1
fi

export STY=$1
SEARCHKEY="FINDME $0 $$"

# Create a new screen window in the given session. 
# The title of this screen will be $SEARCHKEY
# we use 'read a' here to wait until the enter key is pressed.
# We will send the enter key later.
screen -X screen -t "$SEARCHKEY" sh -c 'read a'

# Now search for that window title
window="$(xdotool search --title "$SEARCHKEY")"

# We found the window, let's send a newline to end the 'read a',
# thus closing the added screen window and returning us to our
# previous screen window in that session
screen -X eval 'stuff \012'

if [ -z "$window" ] ; then
  echo "No window found holding screen session $STY"
  echo "Querying...."
  screenls="$(screen -ls $STY)" 
  
  if echo "$screenls" | head -1 | egrep -q '^(This room is empty)|(No )' ; then
    echo "No such screen session: $STY"
  else
    echo "$screenls"
  fi
else
  # Activate the window found
  xdotool windowactivate $window
fi
