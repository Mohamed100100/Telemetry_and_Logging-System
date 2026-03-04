#!/bin/bash

SOCKET_PATH="/tmp/telemetry.sock"
rm -f "$SOCKET_PATH"

echo "Starting RAM socket server at $SOCKET_PATH"
echo "Press Ctrl+C to stop"

while true; do
    # Get used RAM in MB
    USED_MB=$(free -m | awk '/Mem:/ {print $3}')

    # Fallback in case free fails (very rare)
    if [ -z "$USED_MB" ]; then
        USED_MB=$(grep '^MemTotal:' /proc/meminfo | awk '{total=$2}')
    fi

    echo "$USED_MB"
    sleep 1
done | nc -lkU "$SOCKET_PATH"