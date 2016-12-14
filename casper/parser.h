// Defines the generic Parser interface for the Casper parser generator library.
//
#ifndef CASPER_PARSER_H
#define CASPER_PARSER_H

#include <string>
#include <system_error>
#include <type_traits>

namespace casper {

/// The form of a Parser class.  There is no generic implementation of Parser,
/// only partial and full specializations that define how to parse specific
/// grammars.
template <typename Grammar> class Parser;

/// Enumerates the canonical error space for parse results.
enum struct ParseError : int {
  /// The parser ran out of input in the middle of an otherwise succeeding
  /// parse, therefore it is not possible to tell whether the parse would have
  /// succeeded or not had more input been given.
  INCOMPLETE = 1,

  // An unexpected character was encountered.  The returned (Iterator) value
  // will point to the bad character, i.e. one past the last successfully parsed
  // character.
  BAD_CHAR,

  // Some other error occurred.
  UNKNOWN
};

/// The std::error_category implementation for ParseError; allows ParseError
/// values to be automatically converted to and compared with
/// std::error_condition and std::error_code.
class ParseErrorCategory : public std::error_category {
public:
  static ParseErrorCategory &get_instance() {
    static ParseErrorCategory instance;
    return instance;
  }

  const char *name() const noexcept override { return "parse error"; }

  /// This method must contain human-readable messages for all values of
  /// ParseError.
  std::string message(const int c) const override {
    // Do NOT define a default: case!  Doing so will cause the addition of a new
    // ParseError value to NOT fail the compilation, thus allowing the enum and
    // the message table to become out of sync.
    switch (static_cast<ParseError>(c)) {
    case ParseError::INCOMPLETE:
      return "incomplete";
    case ParseError::BAD_CHAR:
      return "unexpected character";
    case ParseError::UNKNOWN:
      return "unknown error";
    }
  }
};

/// Converts a ParseError into a std::error_condition.
std::error_condition make_error_condition(const ParseError e) {
  return std::error_condition{static_cast<int>(e),
                              ParseErrorCategory::get_instance()};
}

/// Converts a ParseError into a std::error_code.
std::error_code make_error_code(const ParseError e) {
  static ParseErrorCategory category;
  return std::error_code{static_cast<int>(e),
                         ParseErrorCategory::get_instance()};
}

template <typename T> class Fallible {
public:
  Fallible() : value_{}, error_{} {}

  /* implicit */ Fallible(T v) : value_{std::move(v)}, error_{} {}

  Fallible(T v, std::error_condition ec)
      : value_{std::move(v)}, error_{std::move(ec)} {}

  const T &get() const { return value_; }

  T &&consume() { return std::move(value_); }

  const std::error_condition &error() const { return error_; }

private:
  T value_;
  std::error_condition error_;
};

/// Equality compare two Fallible values; they need not be exactly the same
/// type, to allow equivalence relations across differing types to be wrapped in
/// Fallible<T>.
template <typename T, typename U>
bool operator==(const Fallible<T> &lhs, const Fallible<U> &rhs) {
  return lhs.get() == rhs.get() && lhs.error() == rhs.error();
}

/// Convenience wrapper around defining a new Parser object and invoking it on
/// the given input range.  Allows the invocation operator defined on
/// Parser<Grammer> to be non-const, as the parser instance created within this
/// function is itself non-const.
template <typename Grammar, typename Range>
auto Parse(const Range &input)
    -> decltype(std::declval<Parser<Grammar>>()(input)) {
  Parser<Grammar> parser;
  return parser(input);
}

} // namespace casper

// Tell the std:: library that ParseError can be converted implicitly to
// error_condition.
//
namespace std {
template <>
struct is_error_condition_enum<casper::ParseError>
    : std::integral_constant<bool, true> {};

} // namespace std

#endif // CASPER_PARSER_H
