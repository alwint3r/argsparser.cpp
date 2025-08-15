#include <climits>
#include <iostream>
#include "argsparser.hpp"

int main(int argc, char* argv[]) {
  argsparser::Parser parser("integer_demo",
                            "Demonstration of new integer argument types");

  // Add arguments for different integer types
  auto* regular_int =
      parser.addArgument<int>("int", "i", "Regular signed integer", false, 0);
  auto* unsigned_int = parser.addArgument<unsigned int>(
      "uint", "u", "Unsigned integer", false, 0u);
  auto* long_int =
      parser.addArgument<long>("long", "l", "Long signed integer", false, 0L);
  auto* unsigned_long = parser.addArgument<unsigned long>(
      "ulong", "g", "Unsigned long integer", false, 0UL);

  // Set validators to show range capabilities
  unsigned_int->setValidator([](unsigned int value) { return value > 0; });
  long_int->setValidator([](long value) { return value != 0; });

  auto result = parser.parse(argc, argv);

  if (result == argsparser::ParseResult::HELP_REQUESTED) {
    return 0;
  }

  if (result != argsparser::ParseResult::SUCCESS) {
    std::cerr << "Error parsing arguments!" << std::endl;
    parser.printHelp();
    return 1;
  }

  std::cout << "Integer Types Demonstration:\n";
  std::cout << "============================\n";

  std::cout << "Regular int value: " << regular_int->getValue() << std::endl;
  std::cout << "Unsigned int value: " << unsigned_int->getValue() << std::endl;
  std::cout << "Long value: " << long_int->getValue() << std::endl;
  std::cout << "Unsigned long value: " << unsigned_long->getValue()
            << std::endl;

  std::cout << "\nType Ranges:\n";
  std::cout << "============\n";
  std::cout << "int range: " << INT_MIN << " to " << INT_MAX << std::endl;
  std::cout << "unsigned int range: 0 to " << UINT_MAX << std::endl;
  std::cout << "long range: " << LONG_MIN << " to " << LONG_MAX << std::endl;
  std::cout << "unsigned long range: 0 to " << ULONG_MAX << std::endl;

  return 0;
}