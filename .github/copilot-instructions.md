# ArgsParser.cpp - C++ Command Line Argument Parser

ArgsParser is a header-only C++ command line argument parser library designed for modern C++ (C++17+) applications with embedded systems in mind. It provides zero-allocation parsing with comprehensive error handling.

**ALWAYS reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.**

## Working Effectively

### Bootstrap and Build the Repository
```bash
# Ensure you have GCC with C++17 support and CMake
g++ --version  # Should be GCC 7+ or equivalent
cmake --version  # Should be CMake 3.10+

# Build using the provided script (recommended)
./build.sh
```

**Build timing**: Takes ~4 seconds total. NEVER CANCEL - set timeout to 30+ seconds minimum for safety.

### Manual Build Process
```bash
# Alternative: Manual CMake build
mkdir -p build
cd build
cmake .. -DCMAKE_CXX_STANDARD=17
make
```

**Build timing**: Takes ~3-4 seconds. NEVER CANCEL - set timeout to 30+ seconds minimum.

### Run Tests
```bash
# Tests run automatically with build.sh, or manually:
cd build
./test_argsparser  # Main functionality tests
./test_overflow    # Integer overflow/underflow tests
```

**Test timing**: Takes <1 second each. Expected output: "All tests passed!" for both.

### Clean Build Artifacts
```bash
./build.sh clean
# OR manually:
rm -rf build
```

## Validation Scenarios

**ALWAYS test these scenarios after making changes to ensure functionality works correctly:**

### Basic Example Application Testing
```bash
cd build

# 1. Test help output
./example --help

# 2. Test basic functionality with all argument types
./example --input input.txt --output result.txt --count 5 --verbose source.txt dest.txt

# 3. Test short options and grouped flags
./example -i input.txt -o result.txt -c 5 -vdq source.txt dest.txt

# 4. Test equals syntax
./example --input=input.txt --output=result.txt --count=5 --verbose source.txt

# 5. Test error scenarios
./example --unknown-option source.txt  # Should show error + help
./example --input input.txt            # Should show missing positional arg error
./example source.txt --count -5        # Should show validator error (count must be positive)
```

**Expected outcomes:**
- Help displays properly formatted usage information
- Valid arguments parse and display correctly
- Error cases show descriptive messages and return exit code 1
- All combinations work with both long (--option) and short (-o) forms

### Manual Compilation Testing
```bash
# Test header-only integration
g++ -std=c++17 -Iinclude examples/example.cpp -o example_manual
./example_manual --help

# Test with strict warnings
g++ -std=c++17 -Iinclude -Wall -Wextra -pedantic tests/test_argsparser.cpp -o test_manual
./test_manual
```

**Expected outcome:** Compilation succeeds, all tests pass. May show minor unused variable warnings in test code - this is normal and tests still pass.

### C++23 Compatibility Testing
```bash
mkdir -p build-cpp23
cd build-cpp23
cmake .. -DCMAKE_CXX_STANDARD=23
make
./test_argsparser && ./test_overflow
```

**Build timing**: Takes ~4 seconds. Works if compiler supports C++23.

## Development Workflow

### Making Changes to the Library
1. **ALWAYS build and test first** to establish baseline:
   ```bash
   ./build.sh
   ```

2. **Edit the header file**: All library code is in `include/argsparser.hpp`

3. **Test your changes**:
   ```bash
   ./build.sh  # Rebuilds and runs all tests
   ```

4. **Validate with example scenarios** (see Validation Scenarios above)

5. **Test manual compilation**:
   ```bash
   g++ -std=c++17 -Iinclude -Wall -Wextra -pedantic examples/example.cpp -o test_changes
   ./test_changes --help
   ```

### Adding New Tests
- Add test functions to `tests/test_argsparser.cpp` or `tests/test_overflow.cpp`
- Follow existing pattern: `void test_feature_name()` with `assert()` statements
- Add call to new test function in `main()`
- Use `std::cout << "test_name passed\n";` for success messages

### Testing Philosophy
- **No external test frameworks** - uses `<cassert>` only
- **All tests must be deterministic** and pass consistently
- **Test both success and error paths**
- **Follow embedded systems principles**: no dynamic allocation, no exceptions

## Common Tasks

### Integration into Your Project

#### Header-Only Integration (Simplest)
```bash
# Copy the header file to your project
cp include/argsparser.hpp your_project/include/
```

#### CMake Integration
```cmake
# In your CMakeLists.txt
add_subdirectory(path/to/argsparser)
target_link_libraries(your_target argsparser)

# Ensure C++17 minimum
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Quick Reference Commands

```bash
# View repository structure
ls -la  # Shows: .git, .gitignore, CMakeLists.txt, QWEN.md, README.md, build.sh, examples/, include/, tests/

# View main header
cat include/argsparser.hpp | head -50  # Header-only library implementation

# View design philosophy
cat QWEN.md  # Important: No exceptions, no dynamic allocation, embedded-friendly

# View example usage
cat examples/example.cpp  # Comprehensive example showing all features

# View test structure  
cat tests/test_argsparser.cpp | head -50  # Assertion-based testing pattern
```

### Repository Structure
```
argsparser.cpp/
├── include/argsparser.hpp          # Main library (header-only)
├── examples/example.cpp            # Comprehensive usage example
├── tests/test_argsparser.cpp       # Main functionality tests
├── tests/test_overflow.cpp         # Integer overflow/underflow tests
├── CMakeLists.txt                  # Build configuration
├── build.sh                        # Build script (recommended)
├── README.md                       # Usage documentation
├── QWEN.md                         # Design philosophy and principles
└── .gitignore                      # Build artifacts exclusion
```

## Troubleshooting

### Common Issues

**Build fails with C++ standard errors:**
```bash
# Ensure GCC 7+ or equivalent
g++ --version
# Explicitly set standard
cmake .. -DCMAKE_CXX_STANDARD=17
```

**Tests fail after changes:**
- Check that your changes don't violate design principles (no dynamic allocation, no exceptions)
- Verify all `assert()` statements in tests
- Run individual test executables to isolate issues

**Example application doesn't work as expected:**
- Check that required arguments are provided
- Verify positional arguments are in correct order
- Use `--help` to see current usage information

### Design Constraints to Remember
- **No dynamic memory allocation** (avoid `new`, `delete`, `std::vector` with default allocator)
- **No exceptions** (use error codes and return values)
- **Embedded systems friendly** (predictable performance, minimal overhead)
- **Header-only design** (all implementation in `argsparser.hpp`)

## Timing Expectations

- **Full build + test**: ~2-4 seconds
- **Individual test run**: <1 second (typically <0.01 seconds)
- **CMake configuration**: <1 second
- **Manual compilation**: <2 seconds

**NEVER CANCEL builds or tests** - they complete very quickly. Set timeouts to 30+ seconds minimum for safety, but expect much faster completion.