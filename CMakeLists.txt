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

# Create C++23 compatibility test executable
add_executable(test_cpp23 tests/test_cpp23.cpp)

# Create example executable
add_executable(example examples/example.cpp)

# Create C++23 example executable
add_executable(cpp23_example examples/cpp23_example.cpp)

# Link the library to the test and example
target_link_libraries(test_argsparser argsparser)
target_link_libraries(test_cpp23 argsparser)
target_link_libraries(example argsparser)
target_link_libraries(cpp23_example argsparser)

# Set C++23 standard for the C++23 test executable
target_compile_features(test_cpp23 PRIVATE cxx_std_23)

# Set C++23 standard for the C++23 example executable
target_compile_features(cpp23_example PRIVATE cxx_std_23)

# Compiler options
if(MSVC)
    target_compile_options(argsparser PRIVATE /W4)
else()
    target_compile_options(argsparser PRIVATE -Wall -Wextra -pedantic)
endif()