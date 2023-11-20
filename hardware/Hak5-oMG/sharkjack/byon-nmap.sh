#!/bin/bash
#
# Title:         Bring your own network NMAP
# Author:        whoisroot
# Version:       1.3
#
# Scans target subnet with Nmap using specified options. Saves each scan result
# to loot storage folder. Includes SERIAL_WRITE commands for Shark Jack Cable.
#
# LED SETUP ... Starting DHCP server
# LED SPECIAL ... Waiting for host to aquire IP address from DHCP
# LED ATTACK ... Scanning
# LED FINISH ... Scan Complete
#
# See nmap --help for options. Default "-sS -sV -p- -O" scans first using
# SYN packets and then scans all TCP ports, verifies reported versions
# listening and determines the OS.


echo "started payload" > /tmp/payload-debug.log
NMAP_OPTIONS="-sS -p- -sV -O --max-retries 3"
LOOT_DIR=/root/loot/BYON_nmap

# Setup loot directory, DHCP server, and wait for host
SERIAL_WRITE [*] Setting up payload
LED SETUP
mkdir -p $LOOT_DIR
COUNT=$(($(ls -l $LOOT_DIR/*.txt | wc -l)+1))
echo "starting DHCP server" >> /tmp/payload-debug.log
SERIAL_WRITE [*] Starting DHCP server
NETMODE DHCP_SERVER
LED SPECIAL
sleep 30
SERIAL_WRITE [*] Waiting for IP from DHCP
while [ -z "$TARGET" ]; do
  sleep 1 && TARGET=$(cut -d" " -f3 /tmp/dhcp.leases)
done
echo -e "Target got IP $TARGET from DHCP\n\n" >> /tmp/payload-debug.log

# Scan target
LED ATTACK
SERIAL_WRITE [*] Starting nmap scan...
nmap -vv $NMAP_OPTIONS -oN $LOOT_DIR/nmap-scan_$COUNT.txt $TARGET 2>&1 | tee -a /tmp/payload-debug.log
echo -e "\n\nscanned host" >> /tmp/payload-debug.log
sleep 2 && sync
LED FINISH
SERIAL_WRITE [*] Payload complete!
