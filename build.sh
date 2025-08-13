#!/bin/bash

# Build script for ArgsParser

echo "Building ArgsParser..."
mkdir -p build
cd build

# Build with default C++ standard (C++17)
echo "Building with default C++ standard..."
cmake ..
make

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Running tests..."
    ./test_argsparser
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "Standard tests passed!"
        
        # Try to build and run C++23 tests if compiler supports it
        echo ""
        echo "Checking for C++23 support..."
        cmake .. -DCMAKE_CXX_STANDARD=23
        if [ $? -eq 0 ]; then
            make test_cpp23
            if [ $? -eq 0 ]; then
                echo ""
                echo "Running C++23 compatibility tests..."
                ./test_cpp23
                
                if [ $? -eq 0 ]; then
                    echo ""
                    echo "C++23 compatibility tests passed!"
                else
                    echo ""
                    echo "C++23 compatibility tests failed or were skipped!"
                fi
            else
                echo ""
                echo "Could not build C++23 tests!"
            fi
        else
            echo ""
            echo "Compiler does not support C++23, skipping C++23 tests."
        fi
        
        echo ""
        echo "You can now run the examples:"
        echo "  ./example --help"
        echo "  ./example --input input.txt --output result.txt --count 5 --verbose"
        echo ""
        echo "If your compiler supports C++23, you can also try:"
        echo "  ./cpp23_example --help"
        echo "  ./cpp23_example --input data.in --output data.out --iterations 10 --verbose"
    else
        echo "Tests failed!"
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi