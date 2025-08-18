#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include "argsparser.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
namespace {
void test_float_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* rate =
      parser.addArgument<float>("rate", "r", "Rate value", false, 1.5F);
  auto* factor = parser.addArgument<float>("factor", "f", "Factor value", true);

  const char* argv[] = {"test_app", "--rate", "3.14", "--factor", "2.718"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("rate"));
  assert(parser.isSet("factor"));

  assert(std::abs(rate->getValue() - 3.14F) < 1e-6F);
  assert(std::abs(factor->getValue() - 2.718F) < 1e-6F);

  std::cout << "test_float_parsing passed\n";
}

void test_double_parsing() {
  argsparser::Parser parser("test_app", "A test application");

  auto* precision = parser.addArgument<double>("precision", "p",
                                               "Precision value", false, 1.0);
  auto* pi = parser.addArgument<double>("pi", "", "Pi value", true);

  const char* argv[] = {"test_app", "--precision", "1e-15", "--pi",
                        "3.141592653589793"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("precision"));
  assert(parser.isSet("pi"));

  assert(std::abs(precision->getValue() - 1e-15) < 1e-16);
  assert(std::abs(pi->getValue() - 3.141592653589793) < 1e-15);

  std::cout << "test_double_parsing passed\n";
}

void test_float_negative_values() {
  argsparser::Parser parser("test_app", "A test application");

  auto* temperature =
      parser.addArgument<float>("temp", "t", "Temperature", true);

  const char* argv[] = {"test_app", "--temp", "-273.15"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("temp"));
  assert(std::abs(temperature->getValue() - (-273.15F)) < 1e-5F);

  std::cout << "test_float_negative_values passed\n";
}

void test_float_scientific_notation() {
  argsparser::Parser parser("test_app", "A test application");

  auto* small = parser.addArgument<double>("small", "s", "Small value", true);
  auto* large = parser.addArgument<double>("large", "l", "Large value", true);

  const char* argv[] = {"test_app", "--small", "1.23e-10", "--large",
                        "4.56E+20"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(parser.isSet("small"));
  assert(parser.isSet("large"));

  assert(std::abs(small->getValue() - 1.23e-10) < 1e-15);
  assert(std::abs(large->getValue() - 4.56e+20) < 1e+15);

  std::cout << "test_float_scientific_notation passed\n";
}

void test_float_invalid_values() {
  argsparser::Parser parser("test_app", "A test application");

  parser.addArgument<float>("value", "v", "Float value", true);

  // Test invalid string
  const char* argv1[] = {"test_app", "--value", "not_a_number"};
  const int argc1 = sizeof(argv1) / sizeof(argv1[0]);

  auto result1 = parser.parse(argc1, const_cast<char**>(argv1));
  assert(result1 == argsparser::ParseResult::INVALID_VALUE);

  // Test partially invalid string
  const char* argv2[] = {"test_app", "--value", "3.14abc"};
  const int argc2 = sizeof(argv2) / sizeof(argv2[0]);

  auto result2 = parser.parse(argc2, const_cast<char**>(argv2));
  assert(result2 == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_float_invalid_values passed\n";
}

void test_float_validator() {
  argsparser::Parser parser("test_app", "A test application");

  auto* percentage =
      parser.addArgument<float>("percent", "p", "Percentage", true);

  // Set validator to ensure percentage is between 0 and 100
  percentage->setValidator(
      [](float value) { return value >= 0.0F && value <= 100.0F; });

  // Test valid percentage
  const char* argv1[] = {"test_app", "--percent", "85.5"};
  const int argc1 = sizeof(argv1) / sizeof(argv1[0]);

  auto result1 = parser.parse(argc1, const_cast<char**>(argv1));
  assert(result1 == argsparser::ParseResult::SUCCESS);
  assert(std::abs(percentage->getValue() - 85.5F) < 1e-6F);

  // Test invalid percentage (too high)
  const char* argv2[] = {"test_app", "--percent", "150.0"};
  const int argc2 = sizeof(argv2) / sizeof(argv2[0]);

  auto result2 = parser.parse(argc2, const_cast<char**>(argv2));
  assert(result2 == argsparser::ParseResult::INVALID_VALUE);

  // Test invalid percentage (negative)
  const char* argv3[] = {"test_app", "--percent", "-10.0"};
  const int argc3 = sizeof(argv3) / sizeof(argv3[0]);

  auto result3 = parser.parse(argc3, const_cast<char**>(argv3));
  assert(result3 == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_float_validator passed\n";
}

void test_double_validator() {
  argsparser::Parser parser("test_app", "A test application");

  auto* threshold =
      parser.addArgument<double>("threshold", "t", "Threshold value", true);

  // Set validator to ensure threshold is positive
  threshold->setValidator([](double value) { return value > 0.0; });

  // Test valid threshold
  const char* argv1[] = {"test_app", "--threshold", "0.0001"};
  const int argc1 = sizeof(argv1) / sizeof(argv1[0]);

  auto result1 = parser.parse(argc1, const_cast<char**>(argv1));
  assert(result1 == argsparser::ParseResult::SUCCESS);
  assert(std::abs(threshold->getValue() - 0.0001) < 1e-10);

  // Test invalid threshold (zero)
  const char* argv2[] = {"test_app", "--threshold", "0.0"};
  const int argc2 = sizeof(argv2) / sizeof(argv2[0]);

  auto result2 = parser.parse(argc2, const_cast<char**>(argv2));
  assert(result2 == argsparser::ParseResult::INVALID_VALUE);

  std::cout << "test_double_validator passed\n";
}

void test_float_default_values() {
  argsparser::Parser parser("test_app", "A test application");

  auto* rate =
      parser.addArgument<float>("rate", "r", "Rate value", false, 2.5F);
  auto* factor =
      parser.addArgument<double>("factor", "f", "Factor value", false, 1.618);

  // Parse without providing values - should use defaults
  const char* argv[] = {"test_app"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(!parser.isSet("rate"));
  assert(!parser.isSet("factor"));

  assert(std::abs(rate->getValue() - 2.5F) < 1e-6F);
  assert(std::abs(factor->getValue() - 1.618) < 1e-10);

  std::cout << "test_float_default_values passed\n";
}

void test_float_help_output() {
  argsparser::Parser parser("test_app", "A test application");

  auto* rate =
      parser.addArgument<float>("rate", "r", "Processing rate", false, 1.5F);
  auto* precision = parser.addArgument<double>("precision", "p",
                                               "Calculation precision", true);

  std::stringstream ss;
  rate->printHelp(ss);
  const std::string rateHelp = ss.str();

  assert(rateHelp.find("-r, --rate") != std::string::npos);
  assert(rateHelp.find("Processing rate") != std::string::npos);
  assert(rateHelp.find("(float)") != std::string::npos);
  assert(rateHelp.find("(default: 1.5)") != std::string::npos);

  ss.str("");
  ss.clear();
  precision->printHelp(ss);
  const std::string precisionHelp = ss.str();

  assert(precisionHelp.find("-p, --precision") != std::string::npos);
  assert(precisionHelp.find("Calculation precision") != std::string::npos);
  assert(precisionHelp.find("(double)") != std::string::npos);
  assert(precisionHelp.find("(required)") != std::string::npos);

  std::cout << "test_float_help_output passed\n";
}

void test_mixed_types() {
  argsparser::Parser parser("test_app", "A test application");

  auto* verbose = parser.addArgument<bool>("verbose", "v", "Verbose output");
  auto* inputFile =
      parser.addArgument<std::string>("input", "i", "Input file", true);
  auto* count = parser.addArgument<int>("count", "c", "Count", false, 10);
  auto* rate = parser.addArgument<float>("rate", "r", "Rate", false, 1.0F);
  auto* precision =
      parser.addArgument<double>("precision", "p", "Precision", false, 1e-6);

  const char* argv[] = {"test_app",    "--input", "data.txt", "--verbose",
                        "--count",     "42",      "--rate",   "3.14",
                        "--precision", "1e-12"};
  const int argc = sizeof(argv) / sizeof(argv[0]);

  auto result = parser.parse(argc, const_cast<char**>(argv));
  assert(result == argsparser::ParseResult::SUCCESS);

  assert(verbose->getValue() == true);
  assert(inputFile->getValue() == "data.txt");
  assert(count->getValue() == 42);
  assert(std::abs(rate->getValue() - 3.14F) < 1e-6F);
  assert(std::abs(precision->getValue() - 1e-12) < 1e-15);

  std::cout << "test_mixed_types passed\n";
}
}  // namespace

int main() {
  test_float_parsing();
  test_double_parsing();
  test_float_negative_values();
  test_float_scientific_notation();
  test_float_invalid_values();
  test_float_validator();
  test_double_validator();
  test_float_default_values();
  test_float_help_output();
  test_mixed_types();

  std::cout << "All floating point tests passed!\n";
  return 0;
}

// NOLINTEND(cppcoreguidelines-pro-type-const-cast)
