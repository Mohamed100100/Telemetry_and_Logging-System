#!/bin/bash

OUTPUT_FILE="/tmp/gpu_data.txt"

echo "Starting GPU monitor..."

# Optional: clear file once at start
> "$OUTPUT_FILE"

# Check if nvidia-smi is available
if ! command -v nvidia-smi &> /dev/null; then
    echo "nvidia-smi not found! Using simulated data."
    
    while true
    do
        GPU_PERCENT=$((RANDOM % 60 + 20))
        
        echo "GPU (simulated): $GPU_PERCENT%"
        echo "$GPU_PERCENT" >> "$OUTPUT_FILE"   # ✅ append
        
        sleep 1
    done
else
    echo "NVIDIA GPU detected."
    
    while true
    do
        GPU_PERCENT=$(nvidia-smi --query-gpu=utilization.gpu --format=csv,noheader,nounits | head -1)
        
        echo "GPU: $GPU_PERCENT%"
        echo "$GPU_PERCENT" >> "$OUTPUT_FILE"   # ✅ append
        
        sleep 1
    done
fi