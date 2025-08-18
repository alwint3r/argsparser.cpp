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


# Discover and analyze header files
print_header "Analyzing header files (*.hpp)"
if [ -d ../include ]; then
    find ../include -type f -name '*.hpp' -print0 | while IFS= read -r -d '' file; do
        echo "--> $(basename "$file")"
        clang-tidy -p build "$file" ${EXTRA_ARGS} 2>&1 | head -100
    done
else
    echo "(No include directory found)"
fi

# Discover and analyze test source files
print_header "Analyzing test sources (tests/*.cpp)"
if [ -d ../tests ]; then
    find ../tests -type f -name '*.cpp' -print0 | while IFS= read -r -d '' file; do
        echo "--> $(basename "$file")"
        clang-tidy -p build "$file" ${EXTRA_ARGS} 2>&1 | head -100
    done
else
    echo "(No tests directory found)"
fi

# Discover and analyze example source files
print_header "Analyzing example sources (examples/*.cpp)"
if [ -d ../examples ]; then
    find ../examples -type f -name '*.cpp' -print0 | while IFS= read -r -d '' file; do
        echo "--> $(basename "$file")"
        clang-tidy -p build "$file" ${EXTRA_ARGS} 2>&1 | head -100
    done
else
    echo "(No examples directory found)"
fi

echo "==== Focused static analysis complete! ===="
echo "Note: Only showing first 100 warnings from each file."
echo "Run clang-tidy manually with specific checks if you want to see all warnings."
