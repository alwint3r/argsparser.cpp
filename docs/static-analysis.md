# Static Analysis Documentation

This document explains how to use static analysis tools with the ArgsParser project.

## Overview

The ArgsParser project includes static analysis capabilities using clang-tidy. Static analysis helps identify potential issues, improve code quality, and maintain consistency across the codebase.

## Prerequisites

Before running static analysis, ensure you have clang-tidy installed:

```bash
# On macOS with Homebrew
brew install llvm

# On Ubuntu/Debian
sudo apt-get install clang-tidy

# On Windows with Chocolatey
choco install llvm
```

## Running Static Analysis

### Method 1: Simplified Static Analysis (Recommended)

Run the simplified static analysis through the build script:

```bash
./build.sh analyze
```

This will:

1. Configure the project with CMake
2. Run clang-tidy with a focused set of checks
3. Analyze key files (header, tests, examples)
4. Show only the most critical warnings

### Method 2: Direct Script Execution

You can also run the static analysis script directly:

```bash
./run-simple-static-analysis.sh
```

### Method 3: Manual Clang-Tidy

For more detailed analysis, you can run clang-tidy manually on specific files:

```bash
# Analyze header file
clang-tidy include/argsparser.hpp -checks="bugprone-*,performance-*,readability-*" -- -Iinclude -std=c++17

# Analyze test file
clang-tidy tests/test_argsparser.cpp -checks="bugprone-*,performance-*,readability-*" -- -Iinclude -std=c++17
```

## Limitations

Due to the project's design choices and use of certain C++ features, some warnings may be produced that are not actual issues. The simplified analysis approach filters out many of these to focus on real problems.

## Customizing Analysis

To customize the static analysis:

1. Edit `.clang-tidy` to modify check settings
2. Modify `run-simple-static-analysis.sh` to change which checks are run
3. Run specific clang-tidy checks manually for deeper analysis

## Integration with IDE

Most modern IDEs support clang-tidy integration:

1. VS Code: Install the "C/C++" extension
2. CLion: Built-in clang-tidy support
3. Vim: Use plugins like ALE or coc-clangd

Configure your IDE to use the project's `.clang-tidy` file for consistent analysis.
