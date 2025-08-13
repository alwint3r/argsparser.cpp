# ArgsParser - Modern C++ Command Line Argument Parser

A simple, header-only command line argument parser library for modern C++ (C++17) applications, designed with embedded systems in mind.

## Features

- Header-only library for easy integration
- Support for various argument types (string, integer, boolean flags)
- Required and optional arguments
- Default values for arguments
- Custom validators for arguments
- Automatic help generation
- Error handling with detailed error codes
- No dynamic memory allocation (except for standard library containers)
- No exceptions (uses error codes instead)

## Usage

### Basic Example

```cpp
#include "argsparser.hpp"

int main(int argc, char* argv[]) {
    argsparser::Parser parser("myapp", "My application description");
    
    // Add a boolean flag
    auto* verbose = parser.addArgument<bool>("verbose", "v", "Enable verbose output");
    
    // Add a required string argument
    auto* inputFile = parser.addArgument<std::string>("input", "i", "Input file path", true);
    
    // Add an optional string argument with default value
    auto* outputFile = parser.addArgument<std::string>("output", "o", "Output file path", false, "output.txt");
    
    // Add an optional integer argument with default value
    auto* count = parser.addArgument<int>("count", "c", "Number of iterations", false, 1);
    
    // Parse command line arguments
    auto result = parser.parse(argc, argv);
    
    // Handle parsing results
    switch (result) {
        case argsparser::ParseResult::HELP_REQUESTED:
            parser.printHelp();
            return 0;
        case argsparser::ParseResult::SUCCESS:
            break;
        // Handle other error cases...
    }
    
    // Use the parsed values
    if (verbose->getValue()) {
        std::cout << "Verbose mode enabled\n";
    }
    std::cout << "Input file: " << inputFile->getValue() << "\n";
    std::cout << "Output file: " << outputFile->getValue() << "\n";
    std::cout << "Count: " << count->getValue() << "\n";
    
    return 0;
}
```

### Adding Custom Validators

```cpp
// Add a validator to ensure count is positive
count->setValidator([](int value) { return value > 0; });
```

## Building

To build the library and example:

```bash
mkdir build
cd build
cmake ..
make
```

## Running Tests

```bash
./test_argsparser
```

### C++23 Compatibility Tests

The library also includes tests for C++23 compatibility:

```bash
# Build with C++23 standard
cmake .. -DCMAKE_CXX_STANDARD=23
make
./test_cpp23
```

## Running the Example

```bash
./example --input input.txt --output result.txt --count 5 --verbose
```

Or using short options:

```bash
./example -i input.txt -o result.txt -c 5 -v
```

For help:

```bash
./example --help
```

### C++23 Example

If your compiler supports C++23, you can also try the C++23 example:

```bash
# Build with C++23 standard
cmake .. -DCMAKE_CXX_STANDARD=23
make
./cpp23_example --input data.in --output data.out --iterations 10 --verbose
```

## Integration

### Header-only Integration

The simplest way to integrate ArgsParser into your project is to copy the header file directly:

1. Copy `include/argsparser.hpp` to your project's include directory
2. Include it in your source files:
   ```cpp
   #include "argsparser.hpp"
   ```

### CMake Integration

If you're using CMake, you can integrate ArgsParser as a subdirectory:

1. Add the ArgsParser repository as a submodule or copy it to your project
2. In your `CMakeLists.txt`:
   ```cmake
   add_subdirectory(path/to/argsparser)
   target_link_libraries(your_target argsparser)
   ```

## Compilation Requirements

ArgsParser requires C++17 or later. Make sure to compile with the appropriate standard:

```bash
g++ -std=c++17 -Iinclude your_source.cpp -o your_executable
```

or with CMake:

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

## C++23 Compatibility

The library is also compatible with C++23. We provide specific tests and examples that demonstrate:

1. Designated initializers (C++20/23 feature)
2. Compatibility with standard library algorithms
3. Use with modern C++ features

To build and run the C++23 tests and examples:

```bash
# Build with C++23 standard
cmake .. -DCMAKE_CXX_STANDARD=23
make
./test_cpp23
./cpp23_example --help
```

## Usage Example

```cpp
#include "argsparser.hpp"

int main(int argc, char* argv[]) {
    argsparser::Parser parser("myapp", "My application description");
    
    // Add arguments
    auto* verbose = parser.addArgument<bool>("verbose", "v", "Enable verbose output");
    auto* inputFile = parser.addArgument<std::string>("input", "i", "Input file path", true);
    
    // Parse command line arguments
    auto result = parser.parse(argc, argv);
    
    // Handle parsing results
    switch (result) {
        case argsparser::ParseResult::HELP_REQUESTED:
            parser.printHelp();
            return 0;
        case argsparser::ParseResult::SUCCESS:
            break;
        // Handle other error cases...
    }
    
    // Use the parsed values
    if (verbose->getValue()) {
        std::cout << "Verbose mode enabled\n";
    }
    std::cout << "Input file: " << inputFile->getValue() << "\n";
    
    return 0;
}
```

## Design Principles

This library follows the design principles outlined in QWEN.md:

1. **Clarity**: Clean, well-documented code
2. **Robustness**: Comprehensive error handling
3. **Maintainability**: Modular design with clear interfaces
4. **Simplicity**: Straightforward API without unnecessary complexity
5. **No dynamic allocation**: Minimizes use of dynamic memory allocation
6. **No exceptions**: Uses error codes instead of exceptions
7. **Modern C++**: Leverages C++17 features where appropriate
