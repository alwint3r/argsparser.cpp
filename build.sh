#!/bin/bash

# Build script for ArgsParser
#
# Functional Requirements:
# 1. Build the ArgsParser library and all associated components
# 2. Support building with multiple C++ standards (C++17 and C++23)
# 3. Compile and run standard tests for the ArgsParser library
# 4. Conditionally compile and run C++23 compatibility tests when supported
# 5. Build example programs to demonstrate library usage
# 6. Provide a clean command to remove build artifacts
#
# Specific behaviors:
# - Creates separate build directories for different C++ standards
# - Builds with C++17 by default
# - Runs standard tests after successful build
# - Attempts to build with C++23 if compiler supports it
# - Runs C++23 compatibility tests if C++23 build is successful
# - Provides usage examples for both standard and C++23 examples
# - Exits with error code if build or tests fail
# - Provides clear output status messages throughout the process
# - Removes build artifacts when invoked with "clean" argument

# Function to clean up build artifacts
cleanup() {
    echo "Cleaning up build artifacts..."
    rm -rf build-cpp17 build-cpp23
    echo "Build artifacts cleaned up successfully!"
}

# Function to run tests
run_tests() {
    echo "Running tests..."
    ./test_argsparser
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "Standard tests passed!"
        return 0
    else
        echo "Tests failed!"
        return 1
    fi
}

# Function to run C++23 tests
run_cpp23_tests() {
    echo ""
    echo "Running C++23 compatibility tests..."
    ./test_cpp23
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "C++23 compatibility tests passed!"
        return 0
    else
        echo ""
        echo "C++23 compatibility tests failed or were skipped!"
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
echo "Building with default C++ standard..."
mkdir -p build-cpp17
cd build-cpp17
cmake ..
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    
    # Run standard tests
    if run_tests; then
        # Try to build and run C++23 tests if compiler supports it
        echo ""
        echo "Checking for C++23 support..."
        mkdir -p ../build-cpp23
        cd ../build-cpp23
        cmake .. -DCMAKE_CXX_STANDARD=23
        if [ $? -eq 0 ]; then
            make
            if [ $? -eq 0 ]; then
                if run_cpp23_tests; then
                    echo ""
                else
                    echo ""
                fi
            else
                echo ""
                echo "Could not build C++23 tests or examples!"
            fi
        else
            echo ""
            echo "Compiler does not support C++23, skipping C++23 tests."
        fi
        
        # Go back to cpp17 build directory to show examples
        cd ../build-cpp17
        
        echo ""
        echo "You can now run the examples:"
        echo "  ./example --help"
        echo "  ./example --input input.txt --output result.txt --count 5 --verbose"
        echo ""
        echo "If your compiler supports C++23, you can also try:"
        echo "  ./cpp23_example --help (from build-cpp23 directory)"
        echo "  ./cpp23_example --input data.in --output data.out --iterations 10 --verbose (from build-cpp23 directory)"
        echo ""
        echo "To clean up build artifacts, run: ./build.sh clean"
    else
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi