#!/bin/sh

prog="$(basename $0)"

# Make sure someserver::homedirs rsync module exists
rsync -q someserver::homedirs
if [ $? -ne 0 ] ; then
  echo "$prog: rsync fatal error" >&2
  exit 1
fi

# For all directories in someserver::homedir that are not dot-directories,
# assume they are a user and try to sync files to their homedir.

rsync someserver::homedirs | awk '$NF !~ /^\./ { print $NF }' \
| while read user; do

  # If the user doesn't exist on this system, skip it.
  getent passwd $user > /dev/null
  if [ $? -ne 0 ] ; then
    echo "$prog: User '$user' does not exist on this server, skipping" >&2
    continue
  fi 

  # Create the home directory first.
  if [ ! -d "/home/$user" ] ; then
    # Get $user's default group
    gid="$(id $user | sed -re 's/^.* gid=([0-9]+)[^0-9].*$/,\1/')"
    install -d -o $user -g $gid -m 0711 /home/$user
  fi
  
  # run rsync as $user to prevent accidents
  su -c "rsync -vr --exclude=.svn someserver::homedirs/$user/ /home/$user/" $user
done

