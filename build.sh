#!/bin/bash

# Build script for ArgsParser
#
# This script builds the ArgsParser library with C++17 standard,
# runs tests, and provides examples of usage.

# Function to clean up build artifacts
cleanup() {
    echo "Cleaning up build artifacts..."
    rm -rf build
    echo "Build artifacts cleaned up successfully!"
}

# Function to run tests
run_tests() {
    echo "Running tests..."
    ./test_argsparser
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "Standard tests passed!"
        
        echo ""
        echo "Running overflow tests..."
        ./test_overflow
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "Overflow tests passed!"
            return 0
        else
            echo ""
            echo "Overflow tests failed!"
            return 1
        fi
    else
        echo "Tests failed!"
        return 1
    fi
}



# Check if cleanup flag is provided
if [ "$1" == "clean" ]; then
    cleanup
    exit 0
fi

echo "Building ArgsParser..."

# Build with default C++ standard (C++17)
echo "Building with C++17 standard..."
mkdir -p build
cd build
cmake .. -DCMAKE_CXX_STANDARD=17
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    
    # Run standard tests
    if run_tests; then
        # Go back to build directory to show examples
        cd ..
        
        echo ""
        echo "You can now run the examples:"
        echo "  ./example --help"
        echo "  ./example --input input.txt --output result.txt --count 5 --verbose"
        echo ""
        echo "To clean up build artifacts, run: ./build.sh clean"
    else
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi