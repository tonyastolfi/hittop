// Definition of ParserError -- canonical errors for the HiTToP parser library
// and support for std::error_condition.
//
#ifndef HITTOP_PARSER_PARSE_ERROR_H
#define HITTOP_PARSER_PARSE_ERROR_H

#include <string>
#include <system_error>

namespace hittop {
namespace parser {

/// Enumerates the canonical error space for parse results.
enum struct ParseError : int {
  /// The parser ran out of input in the middle of an otherwise succeeding
  /// parse, therefore it is not possible to tell whether the parse would have
  /// succeeded or not had more input been given.
  INCOMPLETE = 1,

  // An unexpected character was encountered.  The returned (Iterator) value
  // will point to the bad character, ie. one past the last successfully parsed
  // character.
  BAD_CHAR,

  // For conditional grammar combinators such as Unless, this indicates the
  // condition parser failed.
  FAILED_CONDITION,

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
    case ParseError::FAILED_CONDITION:
      return "exceptional case encountered";
    case ParseError::UNKNOWN:
      return "unknown error";
    }
  }
};

/// Converts a ParseError into a std::error_condition.
inline std::error_condition make_error_condition(const ParseError e) {
  return std::error_condition{static_cast<int>(e),
                              ParseErrorCategory::get_instance()};
}

/// Converts a ParseError into a std::error_code.
inline std::error_code make_error_code(const ParseError e) {
  static ParseErrorCategory category;
  return std::error_code{static_cast<int>(e),
                         ParseErrorCategory::get_instance()};
}

} // namespace parser
} // namespace hittop

// Tell the std:: library that ParseError can be converted implicitly to
// error_condition.
//
namespace std {
template <>
struct is_error_condition_enum<::hittop::parser::ParseError>
    : std::integral_constant<bool, true> {};

} // namespace std

#endif // HITTOP_PARSER_PARSE_ERROR_H
