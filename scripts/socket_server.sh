#!/bin/bash

SOCKET_PATH="/tmp/telemetry_test.sock"

rm -f $SOCKET_PATH

echo "Starting socket server at $SOCKET_PATH"
echo "Press Ctrl+C to stop"

while true; do
    CPU=$((RANDOM % 100))
    MEM=$((RANDOM % 100))
    echo "CPU:${CPU}% MEM:${MEM}%"
    sleep 1
done | nc -lkU $SOCKET_PATH