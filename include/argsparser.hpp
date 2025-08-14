#ifndef ARGSPARSER_HPP
#define ARGSPARSER_HPP

#include <cerrno>   // For errno
#include <climits>  // For INT_MAX and INT_MIN
#include <cstdlib>  // For atoi
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace argsparser {

/**
 * @brief Error codes for argument parsing results
 */
enum class ParseResult {
  SUCCESS = 0,     ///< Parsing completed successfully
  UNKNOWN_OPTION,  ///< An unknown option was provided
  MISSING_VALUE,   ///< A required value is missing
  INVALID_VALUE,   ///< A value is invalid (wrong type or failed validation)
  HELP_REQUESTED   ///< Help was requested (-h or --help)
};

/**
 * @brief Base class for all argument types
 *
 * This abstract class defines the interface for all argument types.
 * Each concrete argument type must implement these methods to handle
 * parsing, validation, and help display.
 */
class ArgumentBase {
 protected:
  std::string name_;
  std::string description_;
  bool isSet_;
  bool isRequired_;

 public:
  /**
   * @brief Construct a new ArgumentBase object
   *
   * @param name The name of the argument
   * @param description The description of the argument
   * @param required Whether the argument is required
   */
  ArgumentBase(const std::string& name, const std::string& description,
               bool required)
      : name_(name),
        description_(description),
        isSet_(false),
        isRequired_(required) {}

  /**
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~ArgumentBase() = default;

  /**
   * @brief Parse a string value into the argument's type
   * @param value The string value to parse
   * @return true if parsing was successful, false otherwise
   */
  virtual bool parse(const std::string& value) = 0;

  /**
   * @brief Print help information for this argument
   * @param os The output stream to print to
   */
  virtual void printHelp(std::ostream& os) const = 0;

  /**
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  bool isSet() const { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  bool isRequired() const { return isRequired_; }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  const std::string& getName() const { return name_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  const std::string& getDescription() const { return description_; }
};

/**
 * @brief Template class for typed arguments
 *
 * This template class provides the base implementation for arguments of any
 * type. Specializations exist for specific types like std::string, bool, and
 * int.
 * @tparam T The type of value this argument holds
 */
template <typename T>
class Argument : public ArgumentBase {
 public:
  /**
   * @brief Validator function type
   *
   * A validator is a function that takes a value of type T and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(const T&)>;

 protected:
  std::string shortName_;
  T value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object
   *
   * @param name The long name of the argument (e.g., "verbose")
   * @param shortName The short name of the argument (e.g., "v")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: T{})
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           const T& defaultValue = T{})
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into this argument's type
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    if (!parseValue(value)) {
      return false;
    }

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return const T& The parsed value
   */
  const T& getValue() const { return value_; }

 private:
  /**
   * @brief Parse a string value into this argument's type (pure virtual)
   *
   * This method must be implemented by specializations to handle
   * type-specific parsing.
   * @param value The string value to parse
   * @return true if parsing was successful, false otherwise
   */
  virtual bool parseValue(const std::string& value) = 0;
};

/**
 * @brief Specialization for string arguments
 *
 * This specialization handles string arguments, which don't require
 * type conversion during parsing.
 */
template <>
class Argument<std::string> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for string arguments
   *
   * A validator is a function that takes a string value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(const std::string&)>;

 private:
  std::string shortName_;
  std::string value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for string values
   *
   * @param name The long name of the argument (e.g., "input")
   * @param shortName The short name of the argument (e.g., "i")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: "")
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           const std::string& defaultValue = "")
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value for this argument
   *
   * For string arguments, parsing is trivial - we just assign the value.
   * @param value The string value to parse
   * @return true if validation was successful, false otherwise
   */
  bool parse(const std::string& value) override {
    if (validator_ && !validator_(value)) {
      return false;
    }

    value_ = value;
    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this string argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_;
    if (!value_.empty()) {
      os << " (default: " << value_ << ")";
    }
    os << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return const std::string& The parsed value
   */
  const std::string& getValue() const { return value_; }
};

/**
 * @brief Specialization for boolean arguments (flags)
 *
 * This specialization handles boolean arguments, which are treated as flags.
 * Flags don't take values; their presence sets them to true.
 */
template <>
class Argument<bool> : public ArgumentBase {
 private:
  std::string shortName_;
  bool value_;

 public:
  /**
   * @brief Construct a new Argument object for boolean values (flags)
   *
   * @param name The long name of the argument (e.g., "--verbose")
   * @param shortName The short name of the argument (e.g., "-v")
   * @param description Description of the argument
   * @param defaultValue Default value for the argument
   * @param required Whether the argument is required (not typically used for
   * flags)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool defaultValue,
           [[maybe_unused]] bool required)
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {
    // Flags are never required
    isRequired_ = false;
  }

  /**
   * @brief Parse a value for this boolean argument (flags)
   *
   * @param value The value to parse (ignored for boolean flags)
   * @return true Always returns true for boolean flags
   */
  bool parse([[maybe_unused]] const std::string& value) override {
    // For flags, we just set to true when present
    value_ = true;
    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this boolean argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    os << "\n    " << description_;
    if (value_) {
      os << " (default: true)";
    }
    os << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return bool The parsed value (true if flag was present)
   */
  bool getValue() const { return value_; }
};

/**
 * @brief Specialization for integer arguments
 *
 * This specialization handles integer arguments, which require
 * conversion from string to int during parsing.
 */
template <>
class Argument<int> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for integer arguments
   *
   * A validator is a function that takes an integer value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(int)>;

 private:
  std::string shortName_;
  int value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           int defaultValue = 0)
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into an integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end;
    errno = 0;  // Reset errno before calling strtol
    long parsedValue = std::strtol(value.c_str(), &end, 10);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow
    if (errno == ERANGE || parsedValue > INT_MAX || parsedValue < INT_MIN) {
      return false;
    }

    value_ = static_cast<int>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this integer argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << " (integer)";
    if (value_ != 0) {
      os << " (default: " << value_ << ")";
    }
    os << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return int The parsed value
   */
  int getValue() const { return value_; }
};

/**
 * @brief Specialization for float arguments
 *
 * This specialization handles float arguments, which require
 * conversion from string to float during parsing.
 */
template <>
class Argument<float> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for float arguments
   *
   * A validator is a function that takes a float value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(float)>;

 private:
  std::string shortName_;
  float value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for float values
   *
   * @param name The long name of the argument (e.g., "rate")
   * @param shortName The short name of the argument (e.g., "r")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0.0f)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           float defaultValue = 0.0f)
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a float
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end;
    errno = 0;  // Reset errno before calling strtof
    float parsedValue = std::strtof(value.c_str(), &end);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow (strtof sets errno to ERANGE)
    if (errno == ERANGE) {
      return false;
    }

    value_ = parsedValue;

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this float argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << " (float)";
    if (value_ != 0.0f) {
      os << " (default: " << value_ << ")";
    }
    os << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return float The parsed value
   */
  float getValue() const { return value_; }
};

/**
 * @brief Specialization for double arguments
 *
 * This specialization handles double arguments, which require
 * conversion from string to double during parsing.
 */
template <>
class Argument<double> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for double arguments
   *
   * A validator is a function that takes a double value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(double)>;

 private:
  std::string shortName_;
  double value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for double values
   *
   * @param name The long name of the argument (e.g., "precision")
   * @param shortName The short name of the argument (e.g., "p")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0.0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           double defaultValue = 0.0)
      : ArgumentBase(name, description, required),
        shortName_(shortName),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a double
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end;
    errno = 0;  // Reset errno before calling strtod
    double parsedValue = std::strtod(value.c_str(), &end);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow (strtod sets errno to ERANGE)
    if (errno == ERANGE) {
      return false;
    }

    value_ = parsedValue;

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Print help information for this double argument
   *
   * @param os The output stream to print to
   */
  void printHelp(std::ostream& os) const override {
    if (!shortName_.empty()) {
      os << "  -" << shortName_ << ", --" << name_;
    } else {
      os << "  --" << name_;
    }
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << " (double)";
    if (value_ != 0.0) {
      os << " (default: " << value_ << ")";
    }
    os << "\n";
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return double The parsed value
   */
  double getValue() const { return value_; }
};

/**
 * @brief Main argument parser class
 *
 * The Parser class is the primary interface for defining and parsing
 * command-line arguments. It manages a collection of arguments and
 * handles the parsing of command-line input.
 */
class Parser {
 private:
  std::string programName_;
  std::string description_;
  std::vector<std::unique_ptr<ArgumentBase>> arguments_;
  std::map<std::string, ArgumentBase*> longNameMap_;
  std::map<std::string, ArgumentBase*> shortNameMap_;
  std::vector<std::unique_ptr<ArgumentBase>> positionalArguments_;
  std::string lastError_;

 public:
  /**
   * @brief Construct a new Parser object
   *
   * @param programName The name of the program (used in help text)
   * @param description A description of the program (used in help text)
   */
  Parser(const std::string& programName, const std::string& description = "")
      : programName_(programName), description_(description) {}

  /**
   * @brief Get the last error message
   *
   * @return const std::string& The last error message
   */
  const std::string& getLastError() const { return lastError_; }

  /**
   * @brief Add a new argument to the parser
   *
   * @tparam T The type of the argument (e.g., bool, std::string, int)
   * @param name The long name of the argument (e.g., "verbose")
   * @param shortName The short name of the argument (e.g., "v")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: T{})
   * @return Argument<T>* Pointer to the created argument
   */
  template <typename T>
  Argument<T>* addArgument(const std::string& name,
                           const std::string& shortName,
                           const std::string& description,
                           bool required = false, const T& defaultValue = T{}) {
    auto arg = std::make_unique<Argument<T>>(name, shortName, description,
                                             required, defaultValue);
    Argument<T>* ptr = arg.get();

    longNameMap_[name] = ptr;
    shortNameMap_[shortName] = ptr;
    arguments_.push_back(std::move(arg));

    return ptr;
  }

  /**
   * @brief Add a new positional argument to the parser
   *
   * @tparam T The type of the argument (e.g., std::string, int)
   * @param name The name of the positional argument
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: true)
   * @param defaultValue The default value for this argument (default: T{})
   * @return Argument<T>* Pointer to the created argument
   */
  template <typename T>
  Argument<T>* addPositionalArgument(const std::string& name,
                                     const std::string& description,
                                     bool required = true,
                                     const T& defaultValue = T{}) {
    auto arg = std::make_unique<Argument<T>>(name, "", description, required,
                                             defaultValue);
    Argument<T>* ptr = arg.get();
    positionalArguments_.push_back(std::move(arg));
    return ptr;
  }

  /**
   * @brief Parse command-line arguments
   *
   * @param argc The number of command-line arguments
   * @param argv The array of command-line argument strings
   * @return ParseResult The result of the parsing operation
   */
  ParseResult parse(int argc, char* argv[]) {
    // Clear the last error
    lastError_.clear();

    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        return ParseResult::HELP_REQUESTED;
      }
    }

    // Collect non-option arguments for positional arguments
    std::vector<std::string> positionalValues;

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];

      if (arg.empty() || arg[0] != '-') {
        // Positional argument
        positionalValues.push_back(arg);
        continue;
      }

      // Handle --option=value syntax
      std::string name;
      std::string value;
      bool hasValue = false;
      bool isLong = (arg.length() > 1 && arg[1] == '-');

      if (isLong) {
        // Long option
        size_t equalPos = arg.find('=');
        if (equalPos != std::string::npos) {
          // --option=value format
          name = arg.substr(2, equalPos - 2);
          value = arg.substr(equalPos + 1);
          hasValue = true;
        } else {
          // --option format
          name = arg.substr(2);
        }
      } else {
        // Short option
        // Check if this is a grouped short option (e.g., -abc) or a short
        // option with a value (e.g., -c123)
        if (arg.length() > 2) {
          // Extract the first character as the short option name
          std::string firstChar = arg.substr(1, 1);
          auto it = shortNameMap_.find(firstChar);

          // Check if the first character corresponds to a non-boolean argument
          if (it != shortNameMap_.end() &&
              !dynamic_cast<Argument<bool>*>(it->second)) {
            // This is a short option with a value (e.g., -c123)
            name = firstChar;
            value = arg.substr(2);
            hasValue = true;
          } else {
            // This might be a grouped short option
            bool isGrouped = true;
            for (size_t j = 1; j < arg.length(); ++j) {
              std::string shortName(1, arg[j]);
              auto it = shortNameMap_.find(shortName);
              if (it == shortNameMap_.end() ||
                  !dynamic_cast<Argument<bool>*>(it->second)) {
                // If any character doesn't correspond to a boolean flag,
                // treat the whole thing as a single short option
                isGrouped = false;
                break;
              }
            }

            if (isGrouped) {
              // Process each character as a separate boolean flag
              for (size_t j = 1; j < arg.length(); ++j) {
                std::string shortName(1, arg[j]);
                auto it = shortNameMap_.find(shortName);
                ArgumentBase* argument = it->second;

                if (!argument->parse("true")) {
                  lastError_ =
                      std::string("Invalid value for flag: -") + shortName;
                  return ParseResult::INVALID_VALUE;
                }
              }
              continue;  // Move to the next argument
            } else {
              // Single short option
              name = arg.substr(1);
            }
          }
        } else {
          // Single short option
          name = arg.substr(1);
        }
      }

      // Find the argument
      ArgumentBase* argument = nullptr;
      if (isLong) {
        auto it = longNameMap_.find(name);
        if (it != longNameMap_.end()) {
          argument = it->second;
        }
      } else {
        auto it = shortNameMap_.find(name);
        if (it != shortNameMap_.end()) {
          argument = it->second;
        }
      }

      if (!argument) {
        lastError_ =
            std::string("Unknown option: ") + (isLong ? "--" : "-") + name;
        return ParseResult::UNKNOWN_OPTION;
      }

      // Handle boolean flags (no value expected)
      if (dynamic_cast<Argument<bool>*>(argument)) {
        if (!argument->parse("true")) {
          lastError_ = std::string("Invalid value for flag: ") +
                       (isLong ? "--" : "-") + name;
          return ParseResult::INVALID_VALUE;
        }
        continue;
      }

      // For non-boolean arguments, get the value
      if (!hasValue) {
        // Expect a value from the next argument
        if (i + 1 >= argc) {
          lastError_ = std::string("Missing value for option: ") +
                       (isLong ? "--" : "-") + name;
          return ParseResult::MISSING_VALUE;
        }
        value = argv[++i];
      }

      if (!argument->parse(value)) {
        lastError_ = std::string("Invalid value for option: ") +
                     (isLong ? "--" : "-") + name + " = " + value;
        return ParseResult::INVALID_VALUE;
      }
    }

    // Parse positional arguments
    size_t positionalIndex = 0;
    for (const auto& arg : positionalArguments_) {
      if (positionalIndex >= positionalValues.size()) {
        if (arg->isRequired()) {
          lastError_ = std::string("Missing required positional argument: ") +
                       arg->getName();
          return ParseResult::MISSING_VALUE;
        }
        // Use default value
        continue;
      }

      if (!arg->parse(positionalValues[positionalIndex])) {
        lastError_ = std::string("Invalid value for positional argument: ") +
                     arg->getName() + " = " + positionalValues[positionalIndex];
        return ParseResult::INVALID_VALUE;
      }
      ++positionalIndex;
    }

    // Check if there are too many positional arguments
    if (positionalIndex < positionalValues.size()) {
      lastError_ = "Too many positional arguments";
      return ParseResult::INVALID_VALUE;
    }

    // Check required option arguments
    for (const auto& arg : arguments_) {
      if (arg->isRequired() && !arg->isSet()) {
        lastError_ =
            std::string("Missing required option: --") + arg->getName();
        return ParseResult::MISSING_VALUE;
      }
    }

    return ParseResult::SUCCESS;
  }

  /**
   * @brief Print help information for all arguments
   *
   * @param os The output stream to print to (default: std::cout)
   */
  void printHelp(std::ostream& os = std::cout) const {
    os << "Usage: " << programName_;

    // Print options
    bool hasOptions = !arguments_.empty();
    if (hasOptions) {
      os << " [OPTIONS]";
    }

    // Print positional arguments
    for (const auto& arg : positionalArguments_) {
      os << " ";
      if (!arg->isRequired()) {
        os << "[";
      }
      os << "<" << arg->getName() << ">";
      if (!arg->isRequired()) {
        os << "]";
      }
    }
    os << "\n";

    if (!description_.empty()) {
      os << description_ << "\n\n";
    }

    // Print option arguments
    if (hasOptions) {
      os << "Options:\n";
      for (const auto& arg : arguments_) {
        arg->printHelp(os);
      }
      os << "\n";
    }

    // Print positional arguments
    if (!positionalArguments_.empty()) {
      os << "Positional arguments:\n";
      for (const auto& arg : positionalArguments_) {
        arg->printHelp(os);
      }
      os << "\n";
    }

    os << "  -h, --help\n    Show this help message\n";
  }

  /**
   * @brief Check if an argument has been set
   *
   * @param name The name of the argument to check
   * @return true if the argument was provided, false otherwise
   */
  bool isSet(const std::string& name) const {
    // Check option arguments first
    auto it = longNameMap_.find(name);
    if (it != longNameMap_.end()) {
      return it->second->isSet();
    }

    // Check positional arguments
    for (const auto& arg : positionalArguments_) {
      if (arg->getName() == name) {
        return arg->isSet();
      }
    }

    return false;
  }

  /**
   * @brief Get the parsed value of an argument
   *
   * @tparam T The type of the argument value
   * @param name The name of the argument
   * @return const T& The parsed value of the argument
   */
  template <typename T>
  const T& getValue(const std::string& name) const {
    // Check option arguments first
    auto it = longNameMap_.find(name);
    if (it != longNameMap_.end()) {
      // This is safe because we know the type matches
      auto* arg = dynamic_cast<Argument<T>*>(it->second);
      if (arg) {
        return arg->getValue();
      }
    }

    // Check positional arguments
    for (const auto& arg : positionalArguments_) {
      if (arg->getName() == name) {
        // This is safe because we know the type matches
        auto* posArg = dynamic_cast<Argument<T>*>(arg.get());
        if (posArg) {
          return posArg->getValue();
        }
      }
    }

    // This should not happen in correct usage
    static T defaultValue{};
    return defaultValue;
  }
};

}  // namespace argsparser

#endif  // ARGSPARSER_HPP
