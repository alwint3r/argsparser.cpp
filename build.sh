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

# Function to run simplified static analysis
run_static_analysis() {
    echo "Running simplified static analysis..."
    
    # Check if clang-tidy is available
    if ! command -v clang-tidy &> /dev/null; then
        echo "Error: clang-tidy is not installed or not in PATH"
        return 1
    fi
    
    # Run our simplified static analysis script
    ./run-simple-static-analysis.sh
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
            
            echo ""
            echo "Running floating point tests..."
            ./test_float
            
            if [ $? -eq 0 ]; then
                echo ""
                echo "Floating point tests passed!"
                
                echo ""
                echo "Running integer types tests..."
                ./test_integer_types
                
                if [ $? -eq 0 ]; then
                    echo ""
                    echo "Integer types tests passed!"
                    return 0
                else
                    echo ""
                    echo "Integer types tests failed!"
                    return 1
                fi
            else
                echo ""
                echo "Floating point tests failed!"
                return 1
            fi
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

# Check if static analysis flag is provided
if [ "$1" == "analyze" ]; then
    run_static_analysis
    exit 0
fi

echo "Building ArgsParser..."

# Build with default C++ standard (C++17)
echo "Building with C++17 standard..."
mkdir -p build
cd build
cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
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
        echo "To run static analysis, run: ./build.sh analyze"
    else
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi