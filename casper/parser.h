#ifndef CASPER_PARSER_H
#define CASPER_PARSER_H

#include <system_error>
#include <type_traits>

namespace casper {

template <typename Grammar> class Parser;

enum struct ParseError : int { INCOMPLETE = 1, BAD_CHAR, UNKNOWN };

class ParseErrorCategory : public std::error_category {
public:
  const char *name() const noexcept override { return "parse error"; }

  std::string message(const int c) const override {
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

std::error_condition make_error_condition(const ParseError e) {
  static ParseErrorCategory category;
  return std::error_condition{static_cast<int>(e), category};
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

template <typename Iterator>
bool operator==(const Fallible<Iterator> &lhs, const Fallible<Iterator> &rhs) {
  return lhs.get() == rhs.get() && lhs.error() == rhs.error();
}

} // namespace casper

namespace std {
template <>
struct is_error_condition_enum<casper::ParseError>
    : std::integral_constant<bool, true> {};

} // namespace std

#endif // CASPER_PARSER_H
