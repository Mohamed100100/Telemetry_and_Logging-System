#!/bin/bash

OUTPUT_FILE="../ram_raw_data.txt"

while true
do
USED_MB=$(free -m | awk '/Mem:/ {print $3}')
echo "USED_MB: $USED_MB"
echo "$USED_MB" >> "$OUTPUT_FILE"
sleep 1
done