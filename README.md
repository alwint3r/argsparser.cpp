# ArgsParser - Modern C++ Command Line Argument Parser

A simple, header-only command line argument parser library for modern C++ (C++17) applications, designed with embedded systems in mind.

## Features

- Header-only library for easy integration
- Support for various argument types (string, fixed-width integers, floating point, boolean flags)
- Required and optional arguments
- Default values for arguments
- Custom validators for arguments
- Automatic help generation
- Error handling with detailed error codes
- Support for `--arg=value` syntax
- Informative error messages
- Support for positional arguments
- Support for grouped short options (e.g., `-abc`)
- Support for short options with values (e.g., `-c123`)
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
    
    // Add an optional integer argument with default value\n    auto* count = parser.addArgument<int32_t>(\"count\", \"c\", \"Number of iterations\", false, 1);
    
    // Add floating point arguments
    auto* rate = parser.addArgument<float>("rate", "r", "Processing rate", false, 1.0f);
    auto* precision = parser.addArgument<double>("precision", "p", "Calculation precision", false, 1e-6);
    
    // Add positional arguments
    auto* sourceFile = parser.addPositionalArgument<std::string>("source", "Source file to process");
    auto* destFile = parser.addPositionalArgument<std::string>("dest", "Destination file", false);
    
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
    std::cout << "Rate: " << rate->getValue() << "\n";
    std::cout << "Precision: " << precision->getValue() << "\n";
    
    if (parser.isSet("source")) {
        std::cout << "Source file: " << sourceFile->getValue() << "\n";
    }
    
    if (parser.isSet("dest")) {
        std::cout << "Destination file: " << destFile->getValue() << "\n";
    }
    
    return 0;
}
```

### Adding Custom Validators

```cpp
// Add a validator to ensure count is positive
count->setValidator([](int32_t value) { return value > 0; });

// Add validators for floating point values
rate->setValidator([](float value) { return value > 0.0f; });
precision->setValidator([](double value) { return value > 0.0 && value < 1.0; });
```

## Building

This is a header-only library, so there's no need to build the library separately. You can simply include the header file in your project:

```cpp
#include "argsparser.hpp"
```

However, to build and run the examples and tests, you can use the provided build script:

```bash
./build.sh
```

This will:
1. Compile and run the standard tests
2. Compile and run the C++23 compatibility tests (if your compiler supports C++23)
3. Build the example programs

To clean up build artifacts:

```bash
./build.sh clean
```

## Code Formatting

This project uses `clang-format` with the Google style guide to ensure consistent code formatting. A pre-commit hook is installed automatically to format C++ files before committing.

To format all C++ files in the project manually, you can run:

```bash
./format.sh
```

If you want to format individual files, you can use `clang-format` directly:

```bash
clang-format -i src/file.cpp include/file.hpp
```

The pre-commit hook will automatically format and stage any C++ files you've modified before committing them.

## Static Analysis

This project includes static analysis capabilities using clang-tidy to help identify potential issues and improve code quality.

To run static analysis:

```bash
./build.sh analyze
```

This will:
1. Configure the project with CMake
2. Run simplified static analysis with clang-tidy
3. Show key warnings and suggestions

For more information on static analysis, see [docs/static-analysis.md](docs/static-analysis.md).

## Running Tests

When you run `./build.sh`, it automatically runs the tests for you. However, if you want to run them manually, you can find the executables in the build directories:

```bash
# For C++17 tests
./build-cpp17/test_argsparser

# For C++23 compatibility tests (if built)
./build-cpp23/test_cpp23
```

## Running the Example

```bash
./example --input input.txt --output result.txt --count 5 --rate 3.14 --precision 1e-6 --verbose --debug --quiet source.txt dest.txt
```

Or using short options:

```bash
./example -i input.txt -o result.txt -c 5 -r 2.5 -p 1e-12 -vdq source.txt dest.txt
```

Or using the `--arg=value` syntax:

```bash
./example --input=input.txt --output=result.txt --count=5 --rate=1.618 --precision=1e-9 --verbose --debug --quiet source.txt dest.txt
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
./build.sh
```

If your compiler supports C++23, the build script will automatically detect this and build the C++23 examples as well.

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

## Roadmap

Here are potential improvements for future versions of ArgsParser:

### High Priority
- **Enhanced Help Output**: Improve the formatting and structure of the help output to make it more visually appealing and easier to read.
- **Multiple Values**: Support for arguments that can take multiple values (e.g., `--file file1.txt --file file2.txt`).
- **Subcommands**: Support for subcommands (e.g., `git commit`, `git push`).

### Medium Priority
- **Argument Dependencies**: Support for specifying dependencies between arguments (e.g., `--output` is only valid if `--input` is specified).
- **Environment Variables**: Support for reading default values from environment variables.
- **Configuration Files**: Support for reading arguments from configuration files (e.g., JSON, INI).
- **Custom Parsers**: Support for custom parsers for complex types.

### Low Priority
- **Argument Groups**: Support for grouping related arguments in the help output.
- **Hidden Arguments**: Support for arguments that are not shown in the help output.
- **Deprecated Arguments**: Support for marking arguments as deprecated with a warning message.
- **Autocomplete**: Support for shell autocomplete generation.