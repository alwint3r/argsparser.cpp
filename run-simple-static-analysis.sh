#!/bin/bash

# Simplified static analysis script for ArgsParser
# This script runs clang-tidy with a focused set of checks for critical issues

echo "Running focused static analysis on ArgsParser project..."

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
EXTRA_ARGS="--extra-arg=-isysroot$(xcrun --sdk macosx --show-sdk-path)"

# Function to print section header
print_header() {
    echo "==== $1 ===="
}

# Run static analysis on header file
print_header "Analyzing header file (argsparser.hpp)"
clang-tidy -p build ../include/argsparser.hpp ${EXTRA_ARGS} 2>&1 | head -100

# # Run static analysis on key test files
# print_header "Analyzing test files (test_argsparser.cpp)"
# clang-tidy -p build ../tests/test_argsparser.cpp ${EXTRA_ARGS} 2>&1 | head -100

# # Run static analysis on example files
# print_header "Analyzing example files (example.cpp)"
# clang-tidy -p build ../examples/example.cpp ${EXTRA_ARGS} 2>&1 | head -100

echo "==== Focused static analysis complete! ===="
echo "Note: Only showing first 100 warnings from each file."
echo "Run clang-tidy manually with specific checks if you want to see all warnings."
