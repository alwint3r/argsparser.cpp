#ifndef ARGSPARSER_HPP
#define ARGSPARSER_HPP

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace argsparser {

/**
 * @brief Error codes for argument parsing results
 */
enum class ParseResult {
  SUCCESS = 0,           ///< Parsing completed successfully
  UNKNOWN_OPTION,        ///< An unknown option was provided
  MISSING_VALUE,         ///< A required value is missing
  INVALID_VALUE,         ///< A value is invalid (wrong type or failed validation)
  HELP_REQUESTED         ///< Help was requested (-h or --help)
};

/**
 * @brief Base class for all argument types
 * 
 * This abstract class defines the interface for all argument types.
 * Each concrete argument type must implement these methods to handle
 * parsing, validation, and help display.
 */
class ArgumentBase {
 public:
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
  virtual bool isSet() const = 0;

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  virtual bool isRequired() const = 0;

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  virtual std::string getDescription() const = 0;
};

/**
 * @brief Template class for typed arguments
 * 
 * This template class provides the base implementation for arguments of any type.
 * Specializations exist for specific types like std::string, bool, and int.
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

 private:
  std::string name_;
  std::string shortName_;
  std::string description_;
  T value_;
  bool isSet_;
  bool isRequired_;
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
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

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
    os << "  -" << shortName_ << ", --" << name_;
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << "\n";
  }

  /**
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  bool isSet() const override { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  bool isRequired() const override { return isRequired_; }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  std::string getName() const override { return name_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  std::string getDescription() const override { return description_; }

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
  std::string name_;
  std::string shortName_;
  std::string description_;
  std::string value_;
  bool isSet_;
  bool isRequired_;
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
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

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
    os << "  -" << shortName_ << ", --" << name_;
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
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  bool isSet() const override { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  bool isRequired() const override { return isRequired_; }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  std::string getName() const override { return name_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  std::string getDescription() const override { return description_; }

  /**
   * @brief Get the parsed value of this argument
   * 
   * @return const std::string& The parsed value
   */
  const std::string& getValue() const { return value_; }

 private:
  /**
   * @brief Parse a string value (trivial implementation)
   * 
   * @param value The string value to parse
   * @return true always (parsing strings never fails)
   */
  bool parseValue(const std::string& value) {
    value_ = value;
    return true;
  }
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
  std::string name_;
  std::string shortName_;
  std::string description_;
  bool value_;
  bool isSet_;

 public:
  /**
   * @brief Construct a new Argument object for boolean values (flags)
   * 
   * @param name The long name of the argument (e.g., "verbose")
   * @param shortName The short name of the argument (e.g., "v")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false, unused for flags)
   * @param defaultValue The default value for this argument (default: false)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           bool defaultValue = false)
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false) {}

  /**
   * @brief Parse a value for this boolean argument (flags)
   * 
   * For flags, parsing just sets the value to true when the flag is present.
   * @param value The string value (ignored for flags)
   * @return true always (parsing flags never fails)
   */
  bool parse(const std::string& value) override {
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
    os << "  -" << shortName_ << ", --" << name_;
    os << "\n    " << description_;
    if (value_) {
      os << " (default: true)";
    }
    os << "\n";
  }

  /**
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  bool isSet() const override { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return false always (flags are never required)
   */
  bool isRequired() const override {
    return false;  // Flags are never required
  }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  std::string getName() const override { return name_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  std::string getDescription() const override { return description_; }

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
  std::string name_;
  std::string shortName_;
  std::string description_;
  int value_;
  bool isSet_;
  bool isRequired_;
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
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

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
    try {
      value_ = std::stoi(value);
    } catch (const std::exception&) {
      return false;
    }

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
    os << "  -" << shortName_ << ", --" << name_;
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
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  bool isSet() const override { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  bool isRequired() const override { return isRequired_; }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  std::string getName() const override { return name_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  std::string getDescription() const override { return description_; }

  /**
   * @brief Get the parsed value of this argument
   * 
   * @return int The parsed value
   */
  int getValue() const { return value_; }

 private:
  /**
   * @brief Parse a string value into an integer
   * 
   * @param value The string value to parse
   * @return true if parsing was successful, false otherwise
   */
  bool parseValue(const std::string& value) {
    try {
      value_ = std::stoi(value);
      return true;
    } catch (const std::exception&) {
      return false;
    }
  }
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
   * @brief Parse command-line arguments
   * 
   * @param argc The number of command-line arguments
   * @param argv The array of command-line argument strings
   * @return ParseResult The result of the parsing operation
   */
  ParseResult parse(int argc, char* argv[]) {
    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
        return ParseResult::HELP_REQUESTED;
      }
    }

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];

      if (arg.empty() || arg[0] != '-') {
        // Positional argument - not supported in this simple implementation
        continue;
      }

      // Remove leading dashes
      bool isLong = (arg.length() > 1 && arg[1] == '-');
      std::string name = isLong ? arg.substr(2) : arg.substr(1);

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
        return ParseResult::UNKNOWN_OPTION;
      }

      // Handle boolean flags (no value expected)
      if (dynamic_cast<Argument<bool>*>(argument)) {
        if (!argument->parse("true")) {
          return ParseResult::INVALID_VALUE;
        }
        continue;
      }

      // For non-boolean arguments, expect a value
      if (i + 1 >= argc) {
        return ParseResult::MISSING_VALUE;
      }

      std::string value = argv[++i];
      if (!argument->parse(value)) {
        return ParseResult::INVALID_VALUE;
      }
    }

    // Check required arguments
    for (const auto& arg : arguments_) {
      if (arg->isRequired() && !arg->isSet()) {
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
    os << "Usage: " << programName_ << " [OPTIONS]\n";
    if (!description_.empty()) {
      os << description_ << "\n\n";
    }

    os << "Options:\n";
    for (const auto& arg : arguments_) {
      arg->printHelp(os);
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
    auto it = longNameMap_.find(name);
    if (it != longNameMap_.end()) {
      return it->second->isSet();
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
    auto it = longNameMap_.find(name);
    if (it != longNameMap_.end()) {
      // This is safe because we know the type matches
      auto* arg = dynamic_cast<Argument<T>*>(it->second);
      if (arg) {
        return arg->getValue();
      }
    }
    // This should not happen in correct usage
    static T defaultValue{};
    return defaultValue;
  }
};

}  // namespace argsparser

#endif  // ARGSPARSER_HPP
