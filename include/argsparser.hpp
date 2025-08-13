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

// Error codes for argument parsing
enum class ParseResult {
  SUCCESS = 0,
  UNKNOWN_OPTION,
  MISSING_VALUE,
  INVALID_VALUE,
  HELP_REQUESTED
};

// Base class for all argument types
class ArgumentBase {
 public:
  virtual ~ArgumentBase() = default;
  virtual bool parse(const std::string& value) = 0;
  virtual void printHelp(std::ostream& os) const = 0;
  virtual bool isSet() const = 0;
  virtual bool isRequired() const = 0;
  virtual std::string getName() const = 0;
  virtual std::string getDescription() const = 0;
};

// Template class for typed arguments
template <typename T>
class Argument : public ArgumentBase {
 public:
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
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           const T& defaultValue = T{})
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

  void setValidator(const Validator& validator) { validator_ = validator; }

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

  void printHelp(std::ostream& os) const override {
    os << "  -" << shortName_ << ", --" << name_;
    if (isRequired_) {
      os << " (required)";
    }
    os << "\n    " << description_ << "\n";
  }

  bool isSet() const override { return isSet_; }

  bool isRequired() const override { return isRequired_; }

  std::string getName() const override { return name_; }

  std::string getDescription() const override { return description_; }

  const T& getValue() const { return value_; }

 private:
  virtual bool parseValue(const std::string& value) = 0;
};

// Specialization for string arguments
template <>
class Argument<std::string> : public ArgumentBase {
 public:
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
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           const std::string& defaultValue = "")
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

  void setValidator(const Validator& validator) { validator_ = validator; }

  bool parse(const std::string& value) override {
    if (validator_ && !validator_(value)) {
      return false;
    }

    value_ = value;
    isSet_ = true;
    return true;
  }

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

  bool isSet() const override { return isSet_; }

  bool isRequired() const override { return isRequired_; }

  std::string getName() const override { return name_; }

  std::string getDescription() const override { return description_; }

  const std::string& getValue() const { return value_; }

 private:
  bool parseValue(const std::string& value) {
    value_ = value;
    return true;
  }
};

// Specialization for boolean arguments (flags)
template <>
class Argument<bool> : public ArgumentBase {
 private:
  std::string name_;
  std::string shortName_;
  std::string description_;
  bool value_;
  bool isSet_;

 public:
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           bool defaultValue = false)
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false) {}

  bool parse(const std::string& value) override {
    // For flags, we just set to true when present
    value_ = true;
    isSet_ = true;
    return true;
  }

  void printHelp(std::ostream& os) const override {
    os << "  -" << shortName_ << ", --" << name_;
    os << "\n    " << description_;
    if (value_) {
      os << " (default: true)";
    }
    os << "\n";
  }

  bool isSet() const override { return isSet_; }

  bool isRequired() const override {
    return false;  // Flags are never required
  }

  std::string getName() const override { return name_; }

  std::string getDescription() const override { return description_; }

  bool getValue() const { return value_; }
};

// Specialization for integer arguments
template <>
class Argument<int> : public ArgumentBase {
 public:
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
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           int defaultValue = 0)
      : name_(name),
        shortName_(shortName),
        description_(description),
        value_(defaultValue),
        isSet_(false),
        isRequired_(required) {}

  void setValidator(const Validator& validator) { validator_ = validator; }

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

  bool isSet() const override { return isSet_; }

  bool isRequired() const override { return isRequired_; }

  std::string getName() const override { return name_; }

  std::string getDescription() const override { return description_; }

  int getValue() const { return value_; }

 private:
  bool parseValue(const std::string& value) {
    try {
      value_ = std::stoi(value);
      return true;
    } catch (const std::exception&) {
      return false;
    }
  }
};

// Main argument parser class
class Parser {
 private:
  std::string programName_;
  std::string description_;
  std::vector<std::unique_ptr<ArgumentBase>> arguments_;
  std::map<std::string, ArgumentBase*> longNameMap_;
  std::map<std::string, ArgumentBase*> shortNameMap_;

 public:
  Parser(const std::string& programName, const std::string& description = "")
      : programName_(programName), description_(description) {}

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

  bool isSet(const std::string& name) const {
    auto it = longNameMap_.find(name);
    if (it != longNameMap_.end()) {
      return it->second->isSet();
    }
    return false;
  }

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
