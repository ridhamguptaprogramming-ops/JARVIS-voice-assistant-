#!/bin/bash

# JARVIS Voice Assistant - Demo Script with Search Features
# This script demonstrates the capabilities of JARVIS including search

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
echo "search for machine learning"
sleep 2
echo "find readme"
sleep 2
echo "tell me about artificial intelligence"
sleep 2
echo "what is blockchain"
sleep 2
echo "tell me a joke"
sleep 1
echo "help"
sleep 1
echo "quit"
) | ./jarvis

echo ""
echo "========================================="
echo "Demo Complete!"
echo "========================================="
