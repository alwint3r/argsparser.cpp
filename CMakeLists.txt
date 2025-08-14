cmake_minimum_required(VERSION 3.10)
project(ArgsParser VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Create library (header-only)
add_library(argsparser INTERFACE)
target_include_directories(argsparser INTERFACE include)

# Create test executable
add_executable(test_argsparser tests/test_argsparser.cpp)

# Create test executable for overflow tests
add_executable(test_overflow tests/test_overflow.cpp)

# Create test executable for floating point tests
add_executable(test_float tests/test_float.cpp)

# Create test executable for integer types tests
add_executable(test_integer_types tests/test_integer_types.cpp)

# Create example executable
add_executable(example examples/example.cpp)

# Create integer demo executable
add_executable(integer_demo examples/integer_demo.cpp)

# For header-only library, we only need to specify include directories
target_include_directories(test_argsparser PRIVATE include)
target_include_directories(test_overflow PRIVATE include)
target_include_directories(test_float PRIVATE include)
target_include_directories(test_integer_types PRIVATE include)
target_include_directories(example PRIVATE include)
target_include_directories(integer_demo PRIVATE include)

# Compiler options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()