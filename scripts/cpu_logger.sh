#!/bin/bash

OUTPUT_FILE="/tmp/cpu_data.txt"

echo "Starting CPU monitor..."

while true
do
    # Method 1: Using top
    CPU_PERCENT=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'.' -f1)
    
    # If top doesn't work, fallback to /proc/stat
    if [ -z "$CPU_PERCENT" ]; then
        CPU_PERCENT=$(grep '^cpu ' /proc/stat | \
        awk '{usage=($2+$4)*100/($2+$4+$5)} END {printf "%.1f", usage}')
    fi
    
    echo "CPU: $CPU_PERCENT%"
    echo "$CPU_PERCENT" > "$OUTPUT_FILE"
    sleep 1
done