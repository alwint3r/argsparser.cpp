#!/bin/bash

# Build script for ArgsParser

echo "Building ArgsParser..."
mkdir -p build
cd build
cmake ..
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Running tests..."
    ./test_argsparser
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "All tests passed!"
        echo ""
        echo "You can now run the example:"
        echo "  ./example --help"
        echo "  ./example --input input.txt --output result.txt --count 5 --verbose"
    else
        echo "Tests failed!"
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi