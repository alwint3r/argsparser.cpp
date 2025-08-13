// C++23 example demonstrating modern C++ features with ArgsParser
// This example will only compile with a C++23-compliant compiler

#if __cplusplus >= 202302L

#include <iostream>
#include <string>
#include <vector>

#include "argsparser.hpp"

int main(int argc, char* argv[]) {
    // Using designated initializers (C++20/23 feature)
    struct AppConfig {
        std::string appName = "cpp23_example";
        std::string description = "C++23 example for ArgsParser";
        bool verbose = false;
        std::string inputFile = "input.txt";
        std::string outputFile = "output.txt";
        int iterations = 10;
    };
    
    // Initialize with designated initializers
    AppConfig config {
        .appName = "cpp23_example",
        .description = "C++23 example demonstrating ArgsParser",
        .verbose = false,  // Default to false for command line
        .inputFile = "data.in",
        .outputFile = "data.out",
        .iterations = 5
    };
    
    // Create parser with the configuration
    argsparser::Parser parser(config.appName, config.description);
    
    // Add arguments
    auto* verbose = parser.addArgument<bool>(
        "verbose", "v", "Enable verbose output", false, config.verbose);
    auto* inputFile = parser.addArgument<std::string>(
        "input", "i", "Input file path", true, config.inputFile);
    auto* outputFile = parser.addArgument<std::string>(
        "output", "o", "Output file path", false, config.outputFile);
    auto* iterations = parser.addArgument<int>(
        "iterations", "n", "Number of iterations", false, config.iterations);
    
    // Parse command line arguments
    auto result = parser.parse(argc, argv);
    
    if (result == argsparser::ParseResult::SUCCESS) {
        std::cout << "C++23 ArgsParser Example\n";
        std::cout << "========================\n";
        std::cout << "Verbose: " << (verbose->getValue() ? "true" : "false") << "\n";
        std::cout << "Input file: " << inputFile->getValue() << "\n";
        std::cout << "Output file: " << outputFile->getValue() << "\n";
        std::cout << "Iterations: " << iterations->getValue() << "\n";
        
        // Demonstrate using standard library algorithms with ArgsParser
        std::cout << "\nArguments summary:\n";
        std::cout << "  - Verbose mode is " << (verbose->getValue() ? "enabled" : "disabled") << "\n";
        std::cout << "  - Processing input file: " << inputFile->getValue() << "\n";
        std::cout << "  - Writing output to: " << outputFile->getValue() << "\n";
        std::cout << "  - Running " << iterations->getValue() << " iterations\n";
        
        return 0;
    } else if (result == argsparser::ParseResult::HELP_REQUESTED) {
        parser.printHelp(std::cout);
        return 0;
    } else {
        std::cerr << "Error parsing arguments\n";
        return 1;
    }
}

#else

#include <iostream>

int main() {
    std::cout << "This example requires C++23 or later to compile.\n";
    std::cout << "Please use a C++23-compliant compiler.\n";
    return 0;
}

#endif