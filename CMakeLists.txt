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

# Create C++23 compatibility test executable
add_executable(test_cpp23 tests/test_cpp23.cpp)

# Create example executable
add_executable(example examples/example.cpp)

# Create C++23 example executable
add_executable(cpp23_example examples/cpp23_example.cpp)

# For header-only library, we only need to specify include directories
target_include_directories(test_argsparser PRIVATE include)
target_include_directories(test_cpp23 PRIVATE include)
target_include_directories(example PRIVATE include)
target_include_directories(cpp23_example PRIVATE include)

# Set C++23 standard for the C++23 test executable
target_compile_features(test_cpp23 PRIVATE cxx_std_23)

# Set C++23 standard for the C++23 example executable
target_compile_features(cpp23_example PRIVATE cxx_std_23)

# Compiler options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()