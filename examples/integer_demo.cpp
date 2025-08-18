#include <cstdint>
#include <iostream>
#include "argsparser.hpp"

int main(int argc, char* argv[]) {
  argsparser::Parser parser("integer_demo",
                            "Demonstration of new integer argument types");

  // Add arguments for different integer types
  auto* regular_int = parser.addArgument<int32_t>(
      "int", "i", "Regular signed integer", false, 0);
  auto* unsigned_int =
      parser.addArgument<uint32_t>("uint", "u", "Unsigned integer", false, 0U);
  auto* long_int = parser.addArgument<int64_t>(
      "long", "l", "Long signed integer", false, 0L);
  auto* unsigned_long = parser.addArgument<uint64_t>(
      "ulong", "g", "Unsigned long integer", false, 0UL);

  // Set validators to show range capabilities
  unsigned_int->setValidator([](uint32_t value) { return value > 0; });
  long_int->setValidator([](int64_t value) { return value != 0; });

  auto result = parser.parse(argc, argv);

  if (result == argsparser::ParseResult::HELP_REQUESTED) {
    return 0;
  }

  if (result != argsparser::ParseResult::SUCCESS) {
    std::cerr << "Error parsing arguments!" << '\n';
    parser.printHelp();
    return 1;
  }

  std::cout << "Integer Types Demonstration:\n";
  std::cout << "============================\n";

  std::cout << "Regular int value: " << regular_int->getValue() << '\n';
  std::cout << "Unsigned int value: " << unsigned_int->getValue() << '\n';
  std::cout << "Long value: " << long_int->getValue() << '\n';
  std::cout << "Unsigned long value: " << unsigned_long->getValue() << '\n';

  std::cout << "\nType Ranges:\n";
  std::cout << "============\n";
  std::cout << "int32_t range: " << INT32_MIN << " to " << INT32_MAX << '\n';
  std::cout << "uint32_t range: 0 to " << UINT32_MAX << '\n';
  std::cout << "int64_t range: " << INT64_MIN << " to " << INT64_MAX << '\n';
  std::cout << "uint64_t range: 0 to " << UINT64_MAX << '\n';

  return 0;
}
