cmake_minimum_required(VERSION 3.10)
project(ArgsParser VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Create library
add_library(argsparser src/argsparser.cpp)

# Create test executable
add_executable(test_argsparser tests/test_argsparser.cpp)

# Create example executable
add_executable(example examples/example.cpp)

# Link the library to the test and example
target_link_libraries(test_argsparser argsparser)
target_link_libraries(example argsparser)

# Compiler options
if(MSVC)
    target_compile_options(argsparser PRIVATE /W4)
else()
    target_compile_options(argsparser PRIVATE -Wall -Wextra -pedantic)
endif()