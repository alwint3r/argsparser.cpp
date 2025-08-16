#!/bin/bash

# Simplified static analysis script for ArgsParser
# This script runs clang-tidy with a minimal set of checks to avoid conflicts

echo "Running simplified static analysis on ArgsParser project..."

# Get the directory of this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}"

# Check if clang-tidy is available
if ! command -v clang-tidy &> /dev/null; then
    echo "Error: clang-tidy is not installed or not in PATH"
    exit 1
fi

# Create build directory if it doesn't exist
if [ ! -d "${PROJECT_ROOT}/build" ]; then
    echo "Creating build directory..."
    mkdir -p "${PROJECT_ROOT}/build"
fi

# Change to build directory
cd "${PROJECT_ROOT}/build"

# Generate compilation database
echo "Configuring project..."
cmake .. -DCMAKE_CXX_STANDARD=17 \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Check if cmake succeeded
if [ $? -ne 0 ]; then
    echo "Error: Failed to configure project with CMake"
    exit 1
fi

# Define a minimal set of checks to avoid conflicts with project design
CHECKS="bugprone-*,performance-*,readability-*,-readability-function-cognitive-complexity,-readability-identifier-naming,-bugprone-easily-swappable-parameters"

# Run static analysis on header file
echo "Analyzing header file..."
clang-tidy ../include/argsparser.hpp \
    -checks="${CHECKS}" \
    --extra-arg=-I../include \
    2>&1 | grep -v "warnings treated as errors" | head -50

# Run static analysis on key test files
echo "Analyzing test files..."
clang-tidy ../tests/test_argsparser.cpp \
    -checks="${CHECKS}" \
    --extra-arg=-I../include \
    2>&1 | grep -v "warnings treated as errors" | head -30

# Run static analysis on example files
echo "Analyzing example files..."
clang-tidy ../examples/example.cpp \
    -checks="${CHECKS}" \
    --extra-arg=-I../include \
    2>&1 | grep -v "warnings treated as errors" | head -30

echo "Simplified static analysis complete!"
echo "Note: Only showing first 50 warnings from header file and first 30 from each test/example file."
echo "Run clang-tidy manually with specific checks if you want to see all warnings."