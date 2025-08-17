#ifndef ARGSPARSER_HPP
#define ARGSPARSER_HPP

#include <array>
#include <cerrno>   // For errno
#include <cstdint>  // For fixed-width integer types
#include <cstdio>   // For snprintf
#include <cstdlib>  // For atoi
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace argsparser {

/**
 * @brief Error codes for argument parsing results
 */
enum class ParseResult : std::uint8_t {
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
  std::string shortName_;
  std::string description_;
  bool isSet_{false};
  bool isRequired_{false};

 public:
  /**
   * @brief Construct a new ArgumentBase object
   *
   * @param name The name of the argument
   * @param shortName The short name of the argument
   * @param description The description of the argument
   * @param required Whether the argument is required
   *
   */ // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  ArgumentBase(const std::string& name, const std::string& shortName,
               const std::string& description, bool required)
      : name_(name),
        shortName_(shortName),
        description_(description),
        isRequired_(required) {}

  /**
   * @brief Virtual destructor for proper cleanup
   */
  virtual ~ArgumentBase() = default;

  // Explicitly declare special member functions to comply with
  // cppcoreguidelines-special-member-functions

  /**
   * @brief Copy constructor (deleted)
   */
  ArgumentBase(const ArgumentBase&) = delete;

  /**
   * @brief Copy assignment operator (deleted)
   */
  ArgumentBase& operator=(const ArgumentBase&) = delete;

  /**
   * @brief Move constructor (deleted)
   */
  ArgumentBase(ArgumentBase&&) = delete;

  /**
   * @brief Move assignment operator (deleted)
   */
  ArgumentBase& operator=(ArgumentBase&&) = delete;

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
  virtual void printHelp(std::ostream& os) const;

  /**
   * @brief Check if this argument has been set
   * @return true if the argument was provided, false otherwise
   */
  [[nodiscard]] bool isSet() const { return isSet_; }

  /**
   * @brief Check if this argument is required
   * @return true if the argument is required, false otherwise
   */
  [[nodiscard]] bool isRequired() const { return isRequired_; }

  /**
   * @brief Get the name of this argument
   * @return The argument's name
   */
  [[nodiscard]] const std::string& getName() const { return name_; }

  /**
   * @brief Get the short name of this argument
   * @return The argument's short name
   */
  [[nodiscard]] const std::string& getShortName() const { return shortName_; }

  /**
   * @brief Get the description of this argument
   * @return The argument's description
   */
  [[nodiscard]] const std::string& getDescription() const {
    return description_;
  }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] virtual std::string getTypeName() const { return ""; }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] virtual std::string getDefaultString() const { return ""; }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] virtual bool hasDefaultValue() const { return false; }
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
      : ArgumentBase(name, shortName, description, required),
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
      : ArgumentBase(name, shortName, description, required),
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
   * @brief Get the parsed value of this argument
   *
   * @return const std::string& The parsed value
   */
  [[nodiscard]] const std::string& getValue() const { return value_; }

 protected:
  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override { return value_; }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override {
    return !value_.empty();
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
  bool value_;

 public:
  /**
   * @brief Construct a new Argument object for boolean values (flags)
   *
   * @param name The long name of the argument (e.g., "verbose")
   * @param shortName The short name of the argument (e.g., "v")
   * @param description Description of the argument
   * @param defaultValue Default value for the argument
   * @param required Whether the argument is required (not typically used for
   * flags)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool defaultValue,
           [[maybe_unused]] bool required)
      : ArgumentBase(name, shortName, description, required),
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
   * @brief Get the parsed value of this argument
   *
   * @return bool The parsed value (true if flag was present)
   */
  [[nodiscard]] bool getValue() const { return value_; }

 protected:
  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return value_ ? "true" : "";
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_; }
};

/**
 * @brief Specialization for 16-bit integer arguments
 *
 * This specialization handles 16-bit integer arguments, which require
 * conversion from string to int16_t during parsing.
 */
template <>
class Argument<int16_t> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for 16-bit integer arguments
   *
   * A validator is a function that takes an int16_t value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(int16_t)>;

 private:
  int16_t value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for 16-bit integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           int16_t defaultValue = 0)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a 16-bit integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtol
    constexpr auto radix = 10;
    const long parsedValue = std::strtol(value.c_str(), &end, radix);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow
    if (errno == ERANGE || parsedValue > INT16_MAX || parsedValue < INT16_MIN) {
      return false;
    }

    value_ = static_cast<int16_t>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return int16_t The parsed value
   */
  [[nodiscard]] int16_t getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override {
    return "(16-bit integer)";
  }

  /**

   * @brief Get the default value as a string

   * @return String representation of the default value or empty string if no

   * default

   */

  [[nodiscard]] std::string getDefaultString() const override {
    return std::to_string(value_);
  }

  /**

   * @brief Check if the argument has a default value that should be displayed

   * @return true if a default value should be shown, false otherwise

   */

  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0; }
};

/**
 * @brief Specialization for 32-bit unsigned integer arguments
 *
 * This specialization handles 32-bit unsigned integer arguments, which require
 * conversion from string to uint32_t during parsing.
 */
template <>
class Argument<uint32_t> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for 32-bit unsigned integer arguments
   *
   * A validator is a function that takes a uint32_t value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(uint32_t)>;

 private:
  uint32_t value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for 32-bit unsigned integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           uint32_t defaultValue = 0)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a 32-bit unsigned integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   * @note Negative values will be rejected even if they would fit in the
   * unsigned type.
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtoul
    constexpr auto radix = 10;
    const unsigned long parsedValue = std::strtoul(value.c_str(), &end, radix);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow or negative values (strtoul would wrap negatives)
    if (errno == ERANGE || parsedValue > UINT32_MAX) {
      return false;
    }

    // Check for negative values by looking at the original string
    if (!value.empty() && value[0] == '-') {
      return false;
    }

    value_ = static_cast<uint32_t>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return uint32_t The parsed value
   */
  [[nodiscard]] uint32_t getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override {
    return "(32-bit unsigned integer)";
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return std::to_string(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0; }
};

/**
 * @brief Specialization for 32-bit integer arguments
 *
 * This specialization handles 32-bit integer arguments, which require
 * conversion from string to int32_t during parsing.
 */
template <>
class Argument<int32_t> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for 32-bit integer arguments
   *
   * A validator is a function that takes an int32_t value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(int32_t)>;

 private:
  int32_t value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for 32-bit integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           int32_t defaultValue = 0)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a 32-bit integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtol
    constexpr auto radix = 10;
    const long parsedValue = std::strtol(value.c_str(), &end, radix);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow
    if (errno == ERANGE || parsedValue > INT32_MAX || parsedValue < INT32_MIN) {
      return false;
    }

    value_ = static_cast<int32_t>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return int32_t The parsed value
   */
  [[nodiscard]] int32_t getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override {
    return "(32-bit integer)";
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return std::to_string(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0; }
};

/**
 * @brief Specialization for 64-bit unsigned integer arguments
 *
 * This specialization handles 64-bit unsigned integer arguments, which require
 * conversion from string to uint64_t during parsing.
 */
template <>
class Argument<uint64_t> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for 64-bit unsigned integer arguments
   *
   * A validator is a function that takes a uint64_t value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(uint64_t)>;

 private:
  uint64_t value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for 64-bit unsigned integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           uint64_t defaultValue = 0)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a 64-bit unsigned integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   * @note Negative values will be rejected even if they would fit in the
   * unsigned type.
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtoull
    constexpr auto radix = 10;
    const unsigned long long parsedValue =
        std::strtoull(value.c_str(), &end, radix);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow or negative values (strtoull would wrap negatives)
    if (errno == ERANGE || parsedValue > UINT64_MAX) {
      return false;
    }

    // Check for negative values by looking at the original string
    if (!value.empty() && value[0] == '-') {
      return false;
    }

    value_ = static_cast<uint64_t>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return uint64_t The parsed value
   */
  [[nodiscard]] uint64_t getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override {
    return "(64-bit unsigned integer)";
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return std::to_string(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0; }
};

/**
 * @brief Specialization for 64-bit integer arguments
 *
 * This specialization handles 64-bit integer arguments, which require
 * conversion from string to int64_t during parsing.
 */
template <>
class Argument<int64_t> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for 64-bit integer arguments
   *
   * A validator is a function that takes an int64_t value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(int64_t)>;

 private:
  int64_t value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for 64-bit integer values
   *
   * @param name The long name of the argument (e.g., "count")
   * @param shortName The short name of the argument (e.g., "c")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           int64_t defaultValue = 0)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a 64-bit integer
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtoll
    constexpr auto radix = 10;
    const long long parsedValue = std::strtoll(value.c_str(), &end, radix);

    // Check if the entire string was consumed
    if (*end != '\0') {
      return false;
    }

    // Check for overflow/underflow
    if (errno == ERANGE || parsedValue > INT64_MAX || parsedValue < INT64_MIN) {
      return false;
    }

    value_ = static_cast<int64_t>(parsedValue);

    if (validator_ && !validator_(value_)) {
      return false;
    }

    isSet_ = true;
    return true;
  }

  /**
   * @brief Get the parsed value of this argument
   *
   * @return int64_t The parsed value
   */
  [[nodiscard]] int64_t getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override {
    return "(64-bit integer)";
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return std::to_string(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0; }
};

/**
 * @brief Specialization for single-precision floating-point arguments
 *
 * This specialization handles single-precision floating-point arguments, which
 * require conversion from string to float during parsing.
 */
template <>
class Argument<float> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for single-precision floating-point
   * arguments
   *
   * A validator is a function that takes a float value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(float)>;

 private:
  float value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for single-precision floating-point
   * values
   *
   * @param name The long name of the argument (e.g., "rate")
   * @param shortName The short name of the argument (e.g., "r")
   * @param description A description of the argument for help text
   * @param required Whether this argument is required (default: false)
   * @param defaultValue The default value for this argument (default: 0.0f)
   */
  Argument(const std::string& name, const std::string& shortName,
           const std::string& description, bool required = false,
           float defaultValue = 0.0F)
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a single-precision floating-point number
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   * @note Handles scientific notation (e.g., 1e-5, 2.5E+3) and standard
   * decimal formats.
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtof
    const float parsedValue = std::strtof(value.c_str(), &end);

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
   * @brief Get the parsed value of this argument
   *
   * @return float The parsed value
   */
  [[nodiscard]] float getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override { return "(float)"; }

  /**
   * @brief Format a float value to a string without trailing zeros
   * @param value The float value to format
   * @return Formatted string representation
   */
  static std::string formatFloat(float value) {
    constexpr size_t size = 32;
    std::array<char, size> buffer{};
    int len = std::snprintf(buffer.data(), buffer.size(), "%.6g", value);
    if (len < 0 || static_cast<std::size_t>(len) >= buffer.size()) {
      // Fallback to scientific notation if buffer is too small
      len = std::snprintf(buffer.data(), buffer.size(), "%.6e", value);
      if (len < 0 || static_cast<std::size_t>(len) >= buffer.size()) {
        return std::string{};
      }
    }
    return {buffer.begin(), buffer.begin() + len};
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return Argument<float>::formatFloat(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0.0F; }
};

/**
 * @brief Specialization for double-precision floating-point arguments
 *
 * This specialization handles double-precision floating-point arguments, which
 * require conversion from string to double during parsing.
 */
template <>
class Argument<double> : public ArgumentBase {
 public:
  /**
   * @brief Validator function type for double-precision floating-point
   * arguments
   *
   * A validator is a function that takes a double value and returns
   * true if the value is valid, false otherwise.
   */
  using Validator = std::function<bool(double)>;

 private:
  double value_;
  Validator validator_;

 public:
  /**
   * @brief Construct a new Argument object for double-precision floating-point
   * values
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
      : ArgumentBase(name, shortName, description, required),
        value_(defaultValue) {}

  /**
   * @brief Set a validator function for this argument
   *
   * The validator function will be called during parsing to validate the value.
   * @param validator The validator function to use
   */
  void setValidator(const Validator& validator) { validator_ = validator; }

  /**
   * @brief Parse a string value into a double-precision floating-point number
   *
   * @param value The string value to parse
   * @return true if parsing and validation were successful, false otherwise
   * @note Handles scientific notation (e.g., 1e-5, 2.5E+3) and standard
   * decimal formats.
   */
  bool parse(const std::string& value) override {
    char* end = nullptr;
    errno = 0;  // Reset errno before calling strtod
    const double parsedValue = std::strtod(value.c_str(), &end);

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
   * @brief Get the parsed value of this argument
   *
   * @return double The parsed value
   */
  [[nodiscard]] double getValue() const { return value_; }

 protected:
  /**
   * @brief Get the type name for this argument (e.g., "(integer)", "(float)")
   * @return The type name or empty string if no type name should be displayed
   */
  [[nodiscard]] std::string getTypeName() const override { return "(double)"; }

  /**
   * @brief Format a double value to a string without trailing zeros
   * @param value The double value to format
   * @return Formatted string representation
   */
  static std::string formatDouble(double value) {
    constexpr size_t size = 32;
    std::array<char, size> buffer{};
    int len = std::snprintf(buffer.data(), buffer.size(), "%.15g", value);
    if (len < 0 || static_cast<std::size_t>(len) >= buffer.size()) {
      // Fallback to scientific notation if buffer is too small
      len = std::snprintf(buffer.data(), buffer.size(), "%.15e", value);
      if (len < 0 || static_cast<std::size_t>(len) >= buffer.size()) {
        return std::string{};
      }
    }
    return {buffer.begin(), buffer.begin() + len};
  }

  /**
   * @brief Get the default value as a string
   * @return String representation of the default value or empty string if no
   * default
   */
  [[nodiscard]] std::string getDefaultString() const override {
    return Argument<double>::formatDouble(value_);
  }

  /**
   * @brief Check if the argument has a default value that should be displayed
   * @return true if a default value should be shown, false otherwise
   */
  [[nodiscard]] bool hasDefaultValue() const override { return value_ != 0.0; }
};

/**
 * @brief Print help information for this argument
 * @param os The output stream to print to
 */
inline void ArgumentBase::printHelp(std::ostream& os) const {
  if (!shortName_.empty()) {
    os << "  -" << shortName_ << ", --" << name_;
  } else {
    os << "  --" << name_;
  }
  if (isRequired_) {
    os << " (required)";
  }
  os << "\n    " << description_;

  // Add type name if applicable
  const std::string typeName = getTypeName();
  if (!typeName.empty()) {
    os << " " << typeName;
  }

  // Add default value if applicable
  if (hasDefaultValue()) {
    const std::string defaultStr = getDefaultString();
    if (!defaultStr.empty()) {
      os << " (default: " << defaultStr << ")";
    }
  }
  os << "\n";
}

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
   */ // NOLINTNEXTLINE(bugprone-easily-swappable-parameters,hicpp-explicit-conversions)
  Parser(const std::string& programName, const std::string& description = "")
      : programName_(programName), description_(description) {}

  /**
   * @brief Get the last error message
   *
   * @return const std::string& The last error message
   */
  [[nodiscard]] const std::string& getLastError() const { return lastError_; }

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
   * @note The returned pointer is owned by the parser and should not be
   * deleted.
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
  Argument<T>*
  addPositionalArgument(const std::string& name, const std::string& description,
                        bool required = true, const T& defaultValue = T{}) {
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
   * @note Supports both long options (--) and short options (-), including
   * grouped short options (-abc) and options with values (--option=value or
   * -ovalue).
   */ // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  ParseResult parse(int argc, char** argv) {
    // Clear the last error
    lastError_.clear();

    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const std::string arg{argv[i]};
      if (arg == "--help" || arg == "-h") {
        return ParseResult::HELP_REQUESTED;
      }
    }

    // Collect non-option arguments for positional arguments
    std::vector<std::string> positionalValues;

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
      const std::string arg{argv[i]};

      if (arg.empty() || arg[0] != '-') {
        // Positional argument
        positionalValues.push_back(arg);
        continue;
      }

      // Handle --option=value syntax
      std::string name;
      std::string value;
      bool hasValue = false;
      const bool isLong = (arg.length() > 1 && arg[1] == '-');

      if (isLong) {
        // Long option
        const size_t equalPos = arg.find('=');
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
          const std::string firstChar = arg.substr(1, 1);
          auto it = shortNameMap_.find(firstChar);

          // Check if the first character corresponds to a non-boolean argument
          if (it != shortNameMap_.end() &&
              dynamic_cast<Argument<bool>*>(it->second) == nullptr) {
            // This is a short option with a value (e.g., -c123)
            name = firstChar;
            value = arg.substr(2);
            hasValue = true;
          } else {
            // This might be a grouped short option
            bool isGrouped = true;
            for (size_t j = 1; j < arg.length(); ++j) {
              const std::string shortName(1, arg[j]);
              auto it = shortNameMap_.find(shortName);
              if (it == shortNameMap_.end() ||
                  dynamic_cast<Argument<bool>*>(it->second) == nullptr) {
                // If any character doesn't correspond to a boolean flag,
                // treat the whole thing as a single short option
                isGrouped = false;
                break;
              }
            }

            if (isGrouped) {
              // Process each character as a separate boolean flag
              for (size_t j = 1; j < arg.length(); ++j) {
                const std::string shortName(1, arg[j]);
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

      if (argument == nullptr) {
        lastError_ =
            std::string("Unknown option: ") + (isLong ? "--" : "-") + name;
        return ParseResult::UNKNOWN_OPTION;
      }

      // Handle boolean flags (no value expected)
      if (dynamic_cast<Argument<bool>*>(argument) != nullptr) {
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
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        value = argv[++i];
      }

      if (!argument->parse(value)) {
        std::string error = "Invalid value for option: ";
        error += (isLong ? "--" : "-");
        error += name;
        error += " = ";
        error += value;
        lastError_ = std::move(error);
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
   * @note The help output includes both option arguments and positional
   * arguments, as well as a usage line showing how to use the program.
   */
  void printHelp(std::ostream& os = std::cout) const {
    os << "Usage: " << programName_;

    // Print options
    const bool hasOptions = !arguments_.empty();
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
   * @note Works for both option arguments and positional arguments.
   */
  [[nodiscard]] bool isSet(const std::string& name) const {
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
   * @note If the argument doesn't exist or the type doesn't match, a default
   * constructed value is returned.
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
