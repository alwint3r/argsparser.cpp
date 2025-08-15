#include <cassert>
#include <climits>
#include <iostream>
#include <string>

#include "argsparser.hpp"

void test_unsigned_int_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count =
      parser.addArgument<unsigned int>("count", "c", "Count value", false, 10u);
  auto* size =
      parser.addArgument<unsigned int>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "100"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42u);
  assert(size->getValue() == 100u);

  std::cout << "test_unsigned_int_parsing passed\n";
}

void test_unsigned_int_negative_values() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count = parser.addArgument<unsigned int>("count", "c", "Count value");

  const char* argv[] = {"test_app", "--count", "-5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_unsigned_int_negative_values passed\n";
}

void test_unsigned_int_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  auto* largeNumber =
      parser.addArgument<unsigned int>("large", "l", "A large number");

  // Test with a number that's too large for unsigned int
  std::string largeValue =
      std::to_string(static_cast<unsigned long long>(UINT_MAX) + 1);
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_unsigned_int_overflow passed\n";
}

void test_unsigned_int_max() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber = parser.addArgument<unsigned int>(
      "max", "m", "Maximum unsigned int value");

  // Test with UINT_MAX, which should work
  std::string maxValue = std::to_string(UINT_MAX);
  const char* argv[] = {"test_app", "--max", maxValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == UINT_MAX);

  std::cout << "test_unsigned_int_max passed\n";
}

void test_long_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count =
      parser.addArgument<long>("count", "c", "Count value", false, 10L);
  auto* size = parser.addArgument<long>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "-100"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42L);
  assert(size->getValue() == -100L);

  std::cout << "test_long_parsing passed\n";
}

void test_long_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  auto* largeNumber = parser.addArgument<long>("large", "l", "A large number");

  // Create a number larger than LONG_MAX
  std::string largeValue =
      std::to_string(LONG_MAX) + "0";  // Add a digit to make it larger
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_long_overflow passed\n";
}

void test_long_underflow() {
  argsparser::Parser parser("test_app", "A test application");

  auto* smallNumber = parser.addArgument<long>("small", "s", "A small number");

  // Create a number smaller than LONG_MIN
  std::string smallValue =
      std::to_string(LONG_MIN) + "0";  // Add a digit to make it smaller
  const char* argv[] = {"test_app", "--small", smallValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_long_underflow passed\n";
}

void test_long_max_min() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber = parser.addArgument<long>("max", "m", "Maximum long value");
  auto* minNumber = parser.addArgument<long>("min", "n", "Minimum long value");

  // Test with LONG_MAX and LONG_MIN, which should work
  std::string maxValue = std::to_string(LONG_MAX);
  std::string minValue = std::to_string(LONG_MIN);
  const char* argv[] = {"test_app", "--max", maxValue.c_str(), "--min",
                        minValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == LONG_MAX);
  assert(minNumber->getValue() == LONG_MIN);

  std::cout << "test_long_max_min passed\n";
}

void test_unsigned_long_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count = parser.addArgument<unsigned long>("count", "c", "Count value",
                                                  false, 10UL);
  auto* size =
      parser.addArgument<unsigned long>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "1000000"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42UL);
  assert(size->getValue() == 1000000UL);

  std::cout << "test_unsigned_long_parsing passed\n";
}

void test_unsigned_long_negative_values() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count = parser.addArgument<unsigned long>("count", "c", "Count value");

  const char* argv[] = {"test_app", "--count", "-5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_unsigned_long_negative_values passed\n";
}

void test_unsigned_long_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  auto* largeNumber =
      parser.addArgument<unsigned long>("large", "l", "A large number");

  // Create a number larger than ULONG_MAX
  std::string largeValue =
      std::to_string(ULONG_MAX) + "0";  // Add a digit to make it larger
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_unsigned_long_overflow passed\n";
}

void test_unsigned_long_max() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber = parser.addArgument<unsigned long>(
      "max", "m", "Maximum unsigned long value");

  // Test with ULONG_MAX, which should work
  std::string maxValue = std::to_string(ULONG_MAX);
  const char* argv[] = {"test_app", "--max", maxValue.c_str()};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == ULONG_MAX);

  std::cout << "test_unsigned_long_max passed\n";
}

void test_integer_types_validators() {
  argsparser::Parser parser("test_app", "A test application");

  auto* positive =
      parser.addArgument<unsigned int>("positive", "p", "Positive value");
  auto* even = parser.addArgument<long>("even", "e", "Even value");
  auto* large = parser.addArgument<unsigned long>("large", "l", "Large value");

  // Set validators
  positive->setValidator([](unsigned int value) { return value > 0; });
  even->setValidator([](long value) { return value % 2 == 0; });
  large->setValidator([](unsigned long value) { return value > 1000000UL; });

  const char* argv[] = {"test_app", "--positive", "5",      "--even",
                        "42",       "--large",    "2000000"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(positive->getValue() == 5u);
  assert(even->getValue() == 42L);
  assert(large->getValue() == 2000000UL);

  std::cout << "test_integer_types_validators passed\n";
}

void test_integer_types_validator_failures() {
  argsparser::Parser parser("test_app", "A test application");

  auto* positive =
      parser.addArgument<unsigned int>("positive", "p", "Positive value");
  positive->setValidator([](unsigned int value) { return value > 0; });

  const char* argv[] = {"test_app", "--positive", "0"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_integer_types_validator_failures passed\n";
}

void test_mixed_integer_types() {
  argsparser::Parser parser("test_app", "A test application");

  auto* regular =
      parser.addArgument<int>("regular", "r", "Regular int", false, 10);
  auto* unsignedInt = parser.addArgument<unsigned int>(
      "unsigned", "u", "Unsigned int", false, 20u);
  auto* longInt = parser.addArgument<long>("long", "l", "Long int", false, 30L);
  auto* unsignedLong = parser.addArgument<unsigned long>(
      "ulong", "g", "Unsigned long", false, 40UL);

  const char* argv[] = {"test_app", "--regular",  "-5",      "--unsigned", "15",
                        "--long",   "-123456789", "--ulong", "987654321"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(regular->getValue() == -5);
  assert(unsignedInt->getValue() == 15u);
  assert(longInt->getValue() == -123456789L);
  assert(unsignedLong->getValue() == 987654321UL);

  std::cout << "test_mixed_integer_types passed\n";
}

int main() {
  test_unsigned_int_parsing();
  test_unsigned_int_negative_values();
  test_unsigned_int_overflow();
  test_unsigned_int_max();
  test_long_parsing();
  test_long_overflow();
  test_long_underflow();
  test_long_max_min();
  test_unsigned_long_parsing();
  test_unsigned_long_negative_values();
  test_unsigned_long_overflow();
  test_unsigned_long_max();
  test_integer_types_validators();
  test_integer_types_validator_failures();
  test_mixed_integer_types();

  std::cout << "All integer types tests passed!\n";
  return 0;
}