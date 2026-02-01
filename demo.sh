#!/bin/bash

# JARVIS Voice Assistant - Demo Script
# This script demonstrates the capabilities of JARVIS

echo "========================================="
echo "JARVIS Voice Assistant - Demo Mode"
echo "========================================="
echo ""

# Build if needed
if [ ! -f "./jarvis" ]; then
    echo "Building JARVIS..."
    make
fi

# Run with test commands
echo "Running JARVIS with demo commands..."
echo ""

(
echo "hello jarvis"
sleep 1
echo "what time is it"
sleep 1
echo "tell me a joke"
sleep 1
echo "help"
sleep 1
echo "system info"
sleep 1
echo "quit"
) | ./jarvis

echo ""
echo "========================================="
echo "Demo Complete!"
echo "========================================="
