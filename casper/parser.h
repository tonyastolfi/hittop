#ifndef CASPER_PARSER_H
#define CASPER_PARSER_H

#include <system_error>

namespace casper {

template <typename Grammar> class parser;

enum Condition : int { OK = 0, INCOMPLETE, BAD_CHAR, UNKNOWN_ERROR };

class ParserError {
public:
  static std::error_category &get_category() {
    static Category c;
    return c;
  }

private:
  struct Category : std::error_category {
    const char *name() const noexcept override { return "parser"; }

    std::string message(const int c) const override {
      switch (static_cast<Condition>(c)) {
      case OK:
        return "ok";
      case INCOMPLETE:
        return "incomplete";
      case BAD_CHAR:
        return "unexpected character";
      case UNKNOWN_ERROR:
        return "unknown error";
      }
    }
  };
};

std::error_condition make_condition(Condition c) {
  return std::error_condition(static_cast<int>(c), ParserError::get_category());
}

template <typename Iterator> struct ParseResult {
  ParseResult() : next{}, condition{make_condition(OK)} {}

  ParseResult(Iterator n, Condition c)
      : next{std::move(n)}, condition{make_condition(c)} {}

  Iterator next;
  std::error_condition condition;
};

template <typename Iterator>
bool operator==(const ParseResult<Iterator> &lhs,
                const ParseResult<Iterator> &rhs) {
  return lhs.next == rhs.next && lhs.condition == rhs.condition;
}

} // namespace casper

#endif // CASPER_PARSER_H
