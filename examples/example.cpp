#include <iostream>

#include "argsparser.hpp"

int main(int argc, char* argv[]) {
  argsparser::Parser parser(
      "example", "An example application demonstrating the argsparser library");

  // Add arguments
  auto* verbose =
      parser.addArgument<bool>("verbose", "v", "Enable verbose output");
  auto* debug = parser.addArgument<bool>("debug", "d", "Enable debug output");
  auto* quiet = parser.addArgument<bool>("quiet", "q", "Suppress output");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);
  auto* outputFile = parser.addArgument<std::string>(
      "output", "o", "Output file path", false, "output.txt");
  auto* count =
      parser.addArgument<int>("count", "c", "Number of iterations", false, 1);
  auto* rate =
      parser.addArgument<float>("rate", "r", "Processing rate", false, 1.0f);
  auto* precision = parser.addArgument<double>(
      "precision", "p", "Calculation precision", false, 1e-6);

  // Add positional arguments
  auto* sourceFile = parser.addPositionalArgument<std::string>(
      "source", "Source file to process");
  auto* destFile = parser.addPositionalArgument<std::string>(
      "dest", "Destination file", false);

  // Add validators
  count->setValidator([](int value) { return value > 0; });
  rate->setValidator([](float value) { return value > 0.0f; });
  precision->setValidator([](double value) { return value > 0.0; });

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
      std::cerr << "Error: " << parser.getLastError() << "\n";
      parser.printHelp(std::cerr);
      return 1;

    case argsparser::ParseResult::MISSING_VALUE:
      std::cerr << "Error: " << parser.getLastError() << "\n";
      parser.printHelp(std::cerr);
      return 1;

    case argsparser::ParseResult::INVALID_VALUE:
      std::cerr << "Error: " << parser.getLastError() << "\n";
      parser.printHelp(std::cerr);
      return 1;
  }

  // Use the parsed values
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

  if (verbose->getValue()) {
    std::cout << "Verbose mode enabled\n";
  }

  if (debug->getValue()) {
    std::cout << "Debug mode enabled\n";
  }

  if (quiet->getValue()) {
    std::cout << "Quiet mode enabled\n";
  }

  // Actual application logic would go here
  std::cout << "Processing complete!\n";

  return 0;
}
