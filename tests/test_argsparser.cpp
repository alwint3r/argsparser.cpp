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
  auto* count = parser.addArgument<int32_t>("count", "c",
                                            "Number of iterations", false, 10);

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

void test_missing_value() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<std::string>("input", "i", "Input file path");

  const char* argv[] = {"test_app", "--input"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::MISSING_VALUE);
  assert(parser.getLastError() == "Missing value for option: --input");

  std::cout << "test_missing_value passed\n";
}

void test_invalid_value() {
  argsparser::Parser parser("test_app", "A test application");
  auto* count =
      parser.addArgument<int32_t>("count", "c", "Number of iterations");

  const char* argv[] = {"test_app", "--count", "not_a_number"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);
  assert(parser.getLastError() ==
         "Invalid value for option: --count = not_a_number");

  std::cout << "test_invalid_value passed\n";
}

void test_validator() {
  argsparser::Parser parser("test_app", "A test application");
  auto* count = parser.addArgument<int32_t>(
      "count", "c", "Number of iterations (must be positive)");
  count->setValidator([](int32_t value) { return value > 0; });

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
  parser.addArgument<int32_t>("count", "c", "Number of iterations", false, 10);

  std::ostringstream oss;
  parser.printHelp(oss);

  std::string helpOutput = oss.str();
  assert(helpOutput.find("Usage: test_app") != std::string::npos);
  assert(helpOutput.find("Enable verbose output") != std::string::npos);
  assert(helpOutput.find("Input file path") != std::string::npos);
  assert(helpOutput.find("default.txt") != std::string::npos);

  std::cout << "test_print_help passed\n";
}

void test_unknown_option() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<bool>("verbose", "v", "Enable verbose output");

  const char* argv[] = {"test_app", "--unknown"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::UNKNOWN_OPTION);
  assert(parser.getLastError() == "Unknown option: --unknown");

  std::cout << "test_unknown_option passed\n";
}

void test_missing_required_option() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addArgument<std::string>("input", "i", "Input file path", true);

  const char* argv[] = {"test_app"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::MISSING_VALUE);
  assert(parser.getLastError() == "Missing required option: --input");

  std::cout << "test_missing_required_option passed\n";
}

void test_equals_syntax() {
  argsparser::Parser parser("test_app", "A test application");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);
  auto* count = parser.addArgument<int32_t>("count", "c",
                                            "Number of iterations", false, 10);

  const char* argv[] = {"test_app", "--input=test.txt", "--count=5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("input"));
  assert(parser.isSet("count"));

  assert(inputFile->getValue() == "test.txt");
  assert(count->getValue() == 5);

  std::cout << "test_equals_syntax passed\n";
}

void test_positional_arguments() {
  argsparser::Parser parser("test_app", "A test application");
  auto* inputFile =
      parser.addPositionalArgument<std::string>("input", "Input file path");
  auto* outputFile = parser.addPositionalArgument<std::string>(
      "output", "Output file path", false, "default.out");
  auto* count = parser.addArgument<int32_t>("count", "c",
                                            "Number of iterations", false, 10);

  const char* argv[] = {"test_app", "input.txt", "output.txt", "--count=5"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("input"));
  assert(parser.isSet("output"));
  assert(parser.isSet("count"));

  assert(inputFile->getValue() == "input.txt");
  assert(outputFile->getValue() == "output.txt");
  assert(count->getValue() == 5);

  std::cout << "test_positional_arguments passed\n";
}

void test_missing_positional_argument() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addPositionalArgument<std::string>("input", "Input file path");

  const char* argv[] = {"test_app"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::MISSING_VALUE);
  assert(parser.getLastError() ==
         "Missing required positional argument: input");

  std::cout << "test_missing_positional_argument passed\n";
}

void test_too_many_positional_arguments() {
  argsparser::Parser parser("test_app", "A test application");
  parser.addPositionalArgument<std::string>("input", "Input file path");

  const char* argv[] = {"test_app", "input.txt", "extra.txt"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::INVALID_VALUE);
  assert(parser.getLastError() == "Too many positional arguments");

  std::cout << "test_too_many_positional_arguments passed\n";
}

void test_grouped_short_options() {
  argsparser::Parser parser("test_app", "A test application");
  auto* verbose =
      parser.addArgument<bool>("verbose", "v", "Enable verbose output");
  auto* debug = parser.addArgument<bool>("debug", "d", "Enable debug output");
  auto* quiet = parser.addArgument<bool>("quiet", "q", "Suppress output");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);

  const char* argv[] = {"test_app", "-vdq", "--input", "test.txt"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("verbose"));
  assert(parser.isSet("debug"));
  assert(parser.isSet("quiet"));
  assert(parser.isSet("input"));

  assert(verbose->getValue() == true);
  assert(debug->getValue() == true);
  assert(quiet->getValue() == true);
  assert(inputFile->getValue() == "test.txt");

  std::cout << "test_grouped_short_options passed\n";
}

void test_grouped_short_options_with_non_bool() {
  argsparser::Parser parser("test_app", "A test application");
  auto* verbose =
      parser.addArgument<bool>("verbose", "v", "Enable verbose output");
  auto* count =
      parser.addArgument<int32_t>("count", "c", "Number of iterations");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file path", true);

  // This should be treated as -c with value 123
  const char* argv[] = {"test_app", "-c123", "--input", "test.txt"};
  int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("count"));
  assert(parser.isSet("input"));

  assert(count->getValue() == 123);
  assert(inputFile->getValue() == "test.txt");

  std::cout << "test_grouped_short_options_with_non_bool passed\n";
}

int main() {
  test_basic_parsing();
  test_help_request();
  test_missing_required_option();
  test_missing_value();
  test_invalid_value();
  test_validator();
  test_print_help();
  test_unknown_option();
  test_equals_syntax();
  test_positional_arguments();
  test_missing_positional_argument();
  test_too_many_positional_arguments();
  test_grouped_short_options();
  test_grouped_short_options_with_non_bool();

  std::cout << "All tests passed!\n";
  return 0;
}
