#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "argsparser.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
namespace {
void test_uint32_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count =
      parser.addArgument<uint32_t>("count", "c", "Count value", false, 10U);
  auto* size = parser.addArgument<uint32_t>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "100"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42U);
  assert(size->getValue() == 100U);

  std::cout << "test_uint32_parsing passed\n";
}

void test_uint32_negative_values() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<uint32_t>("count", "c", "Count value");

  const char* argv[] = {"test_app", "--count", "-5"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_uint32_negative_values passed\n";
}

void test_uint32_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<uint32_t>("large", "l", "A large number");

  // Test with a number that's too large for uint32_t
  const std::string largeValue =
      std::to_string(static_cast<unsigned long long>(UINT32_MAX) + 1);
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_uint32_overflow passed\n";
}

void test_uint32_max() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber =
      parser.addArgument<uint32_t>("max", "m", "Maximum uint32_t value");

  // Test with UINT32_MAX, which should work
  const std::string maxValue = std::to_string(UINT32_MAX);
  const char* argv[] = {"test_app", "--max", maxValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == UINT32_MAX);

  std::cout << "test_uint32_max passed\n";
}

void test_int64_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count =
      parser.addArgument<int64_t>("count", "c", "Count value", false, 10L);
  auto* size = parser.addArgument<int64_t>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "-100"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42L);
  assert(size->getValue() == -100L);

  std::cout << "test_int64_parsing passed\n";
}

void test_int64_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<int64_t>("large", "l", "A large number");

  // Create a number larger than INT64_MAX
  const std::string largeValue =
      std::to_string(INT64_MAX) + "0";  // Add a digit to make it larger
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_int64_overflow passed\n";
}

void test_int64_underflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<int64_t>("small", "s", "A small number");

  // Create a number smaller than INT64_MIN
  const std::string smallValue =
      std::to_string(INT64_MIN) + "0";  // Add a digit to make it smaller
  const char* argv[] = {"test_app", "--small", smallValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_int64_underflow passed\n";
}

void test_int64_max_min() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber =
      parser.addArgument<int64_t>("max", "m", "Maximum int64_t value");
  auto* minNumber =
      parser.addArgument<int64_t>("min", "n", "Minimum int64_t value");

  // Test with INT64_MAX and INT64_MIN, which should work
  const std::string maxValue = std::to_string(INT64_MAX);
  const std::string minValue = std::to_string(INT64_MIN);
  const char* argv[] = {"test_app", "--max", maxValue.c_str(), "--min",
                        minValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == INT64_MAX);
  assert(minNumber->getValue() == INT64_MIN);

  std::cout << "test_int64_max_min passed\n";
}

void test_uint64_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* count =
      parser.addArgument<uint64_t>("count", "c", "Count value", false, 10UL);
  auto* size = parser.addArgument<uint64_t>("size", "s", "Size value", true);

  const char* argv[] = {"test_app", "--count", "42", "--size", "1000000"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("size"));

  assert(count->getValue() == 42UL);
  assert(size->getValue() == 1000000UL);

  std::cout << "test_uint64_parsing passed\n";
}

void test_uint64_negative_values() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<uint64_t>("count", "c", "Count value");

  const char* argv[] = {"test_app", "--count", "-5"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_uint64_negative_values passed\n";
}

void test_uint64_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<uint64_t>("large", "l", "A large number");

  // Create a number larger than UINT64_MAX
  const std::string largeValue =
      std::to_string(UINT64_MAX) + "0";  // Add a digit to make it larger
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_uint64_overflow passed\n";
}

void test_uint64_max() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber =
      parser.addArgument<uint64_t>("max", "m", "Maximum uint64_t value");

  // Test with UINT64_MAX, which should work
  const std::string maxValue = std::to_string(UINT64_MAX);
  const char* argv[] = {"test_app", "--max", maxValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == UINT64_MAX);

  std::cout << "test_uint64_max passed\n";
}

void test_integer_types_validators() {
  argsparser::Parser parser("test_app", "A test application");

  auto* positive =
      parser.addArgument<uint32_t>("positive", "p", "Positive value");
  auto* even = parser.addArgument<int64_t>("even", "e", "Even value");
  auto* large = parser.addArgument<uint64_t>("large", "l", "Large value");

  // Set validators
  positive->setValidator([](uint32_t value) { return value > 0; });
  even->setValidator([](int64_t value) { return value % 2 == 0; });
  large->setValidator([](uint64_t value) { return value > 1000000UL; });

  const char* argv[] = {"test_app", "--positive", "5",      "--even",
                        "42",       "--large",    "2000000"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(positive->getValue() == 5U);
  assert(even->getValue() == 42L);
  assert(large->getValue() == 2000000UL);

  std::cout << "test_integer_types_validators passed\n";
}

void test_integer_types_validator_failures() {
  argsparser::Parser parser("test_app", "A test application");

  auto* positive =
      parser.addArgument<uint32_t>("positive", "p", "Positive value");
  positive->setValidator([](uint32_t value) { return value > 0; });

  const char* argv[] = {"test_app", "--positive", "0"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_integer_types_validator_failures passed\n";
}

void test_mixed_integer_types() {
  argsparser::Parser parser("test_app", "A test application");

  auto* regular =
      parser.addArgument<int32_t>("regular", "r", "Regular int", false, 10);
  auto* unsignedInt =
      parser.addArgument<uint32_t>("unsigned", "u", "Unsigned int", false, 20U);
  auto* longInt =
      parser.addArgument<int64_t>("long", "l", "Long int", false, 30L);
  auto* unsignedLong =
      parser.addArgument<uint64_t>("ulong", "g", "Unsigned long", false, 40UL);

  const char* argv[] = {"test_app", "--regular",  "-5",      "--unsigned", "15",
                        "--long",   "-123456789", "--ulong", "987654321"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(regular->getValue() == -5);
  assert(unsignedInt->getValue() == 15U);
  assert(longInt->getValue() == -123456789L);
  assert(unsignedLong->getValue() == 987654321UL);

  std::cout << "test_mixed_integer_types passed\n";
}
}  // namespace

int main() {
  test_uint32_parsing();
  test_uint32_negative_values();
  test_uint32_overflow();
  test_uint32_max();
  test_int64_parsing();
  test_int64_overflow();
  test_int64_underflow();
  test_int64_max_min();
  test_uint64_parsing();
  test_uint64_negative_values();
  test_uint64_overflow();
  test_uint64_max();
  test_integer_types_validators();
  test_integer_types_validator_failures();
  test_mixed_integer_types();

  std::cout << "All integer types tests passed!\n";
  return 0;
}
// NOLINTEND(cppcoreguidelines-pro-type-const-cast)
