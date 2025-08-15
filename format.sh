#!/bin/bash

# Format all C++ files in the project with clang-format

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "clang-format is not installed. Please install it to enable formatting."
    exit 1
fi

# Find all C++ files and format them
find . -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.h" | \
    grep -v "/build/" | \
    xargs clang-format -i

echo "All C++ files have been formatted with clang-format."