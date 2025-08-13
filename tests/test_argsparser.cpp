#include <cassert>
#include <iostream>
#include <sstream>

#include "argsparser.hpp"

void test_basic_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* verbose = parser.addArgument<bool>(
      "verbose", "v", "Enable verbose output", false, false);
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);
  auto* count =
      parser.addArgument<int>("count", "c", "Number of iterations", false, 10);

  const char* argv[] = {"test_app", "--input", "test.txt", "-v", "-c", "5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("verbose"));
  assert(parser.isSet("input"));
  assert(parser.isSet("count"));

  assert(verbose->getValue() == true);
  assert(inputFile->getValue() == "test.txt");
  assert(count->getValue() == 5);

  std::cout << "test_basic_parsing passed\n";
}

void test_help_request() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<bool>("verbose", "v", "Enable verbose output");

  const char* argv[] = {"test_app", "--help"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::HELP_REQUESTED);

  std::cout << "test_help_request passed\n";
}

void test_missing_required_argument() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<std::string>("input", "i", "Input file path", true);

  const char* argv[] = {"test_app"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::MISSING_VALUE);

  std::cout << "test_missing_required_argument passed\n";
}

void test_invalid_value() {
  argsparser::Parser parser("test_app", "A test application");
  auto* count = parser.addArgument<int>("count", "c", "Number of iterations");

  const char* argv[] = {"test_app", "--count", "not_a_number"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_invalid_value passed\n";
}

void test_validator() {
  argsparser::Parser parser("test_app", "A test application");
  auto* count = parser.addArgument<int>(
      "count", "c", "Number of iterations (must be positive)");
  count->setValidator([](int value) { return value > 0; });

  const char* argv[] = {"test_app", "--count", "-5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_validator passed\n";
}

void test_print_help() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<bool>("verbose", "v", "Enable verbose output");
  parser.addArgument<std::string>("input", "i", "Input file path", true,
                                  "default.txt");
  parser.addArgument<int>("count", "c", "Number of iterations", false, 10);

  std::ostringstream oss;
  parser.printHelp(oss);

  std::string helpOutput = oss.str();
  assert(helpOutput.find("Usage: test_app") != std::string::npos);
  assert(helpOutput.find("Enable verbose output") != std::string::npos);
  assert(helpOutput.find("Input file path") != std::string::npos);
  assert(helpOutput.find("default.txt") != std::string::npos);

  std::cout << "test_print_help passed\n";
}

int main() {
  test_basic_parsing();
  test_help_request();
  test_missing_required_argument();
  test_invalid_value();
  test_validator();
  test_print_help();

  std::cout << "All tests passed!\n";
  return 0;
}
