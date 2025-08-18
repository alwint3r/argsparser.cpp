#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "argsparser.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
namespace {
void test_integer_overflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<int32_t>("large", "l", "A large number");

  // Test with a number that's too large for int32_t
  const std::string largeValue =
      std::to_string(static_cast<long long>(INT32_MAX) + 1);
  const char* argv[] = {"test_app", "--large", largeValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_integer_overflow passed\n";
}

void test_integer_underflow() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<int32_t>("small", "s", "A small number");

  // Test with a number that's too small for int32_t
  const std::string smallValue =
      std::to_string(static_cast<long long>(INT32_MIN) - 1);
  const char* argv[] = {"test_app", "--small", smallValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_integer_underflow passed\n";
}

void test_integer_max() {
  argsparser::Parser parser("test_app", "A test application");

  auto* maxNumber =
      parser.addArgument<int32_t>("max", "m", "Maximum int32_t value");

  // Test with INT32_MAX, which should work
  const std::string maxValue = std::to_string(INT32_MAX);
  const char* argv[] = {"test_app", "--max", maxValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(maxNumber->getValue() == INT32_MAX);

  std::cout << "test_integer_max passed\n";
}

void test_integer_min() {
  argsparser::Parser parser("test_app", "A test application");

  auto* minNumber =
      parser.addArgument<int32_t>("min", "n", "Minimum int32_t value");

  // Test with INT32_MIN, which should work
  const std::string minValue = std::to_string(INT32_MIN);
  const char* argv[] = {"test_app", "--min", minValue.c_str()};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);
  assert(minNumber->getValue() == INT32_MIN);

  std::cout << "test_integer_min passed\n";
}
}  // namespace

int main() {
  test_integer_overflow();
  test_integer_underflow();
  test_integer_max();
  test_integer_min();

  std::cout << "All overflow tests passed!\n";
  return 0;
}
// NOLINTEND(cppcoreguidelines-pro-type-const-cast)
