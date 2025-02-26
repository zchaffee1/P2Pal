#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Remove old build files
rm -rf build/*

# Run CMake build
cmake -B build
cmake --build build

# Run the application with option of Port Number
if [[ -n "$1" ]]; then
    ./build/p2pal-chat "$1"
else
    ./build/p2pal-chat
fi

