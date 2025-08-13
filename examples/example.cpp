#include <iostream>

#include "argsparser.hpp"

int main(int argc, char* argv[]) {
  argsparser::Parser parser(
      "example", "An example application demonstrating the argsparser library");

  // Add arguments
  auto* verbose =
      parser.addArgument<bool>("verbose", "v", "Enable verbose output");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);
  auto* outputFile = parser.addArgument<std::string>(
      "output", "o", "Output file path", false, "output.txt");
  auto* count =
      parser.addArgument<int>("count", "c", "Number of iterations", false, 1);

  // Add a validator to ensure count is positive
  count->setValidator([](int value) { return value > 0; });

  // Parse command line arguments
  auto result = parser.parse(argc, argv);

  // Handle parsing results
  switch (result) {
    case argsparser::ParseResult::HELP_REQUESTED:
      parser.printHelp();
      return 0;

    case argsparser::ParseResult::SUCCESS:
      break;

    case argsparser::ParseResult::UNKNOWN_OPTION:
      std::cerr << "Error: Unknown option provided\n";
      parser.printHelp(std::cerr);
      return 1;

    case argsparser::ParseResult::MISSING_VALUE:
      std::cerr << "Error: Missing required value\n";
      parser.printHelp(std::cerr);
      return 1;

    case argsparser::ParseResult::INVALID_VALUE:
      std::cerr << "Error: Invalid value provided\n";
      parser.printHelp(std::cerr);
      return 1;
  }

  // Use the parsed values
  std::cout << "Input file: " << inputFile->getValue() << "\n";
  std::cout << "Output file: " << outputFile->getValue() << "\n";
  std::cout << "Count: " << count->getValue() << "\n";

  if (verbose->getValue()) {
    std::cout << "Verbose mode enabled\n";
  }

  // Actual application logic would go here
  std::cout << "Processing complete!\n";

  return 0;
}
