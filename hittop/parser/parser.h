// Defines the generic Parser interface for the Casper parser generator library.
//
#ifndef HITTOP_PARSER_PARSER_H
#define HITTOP_PARSER_PARSER_H

#include <iostream>

#include "boost/range/as_literal.hpp"

#include "hittop/parser/parse_error.h"
#include "hittop/parser/traits.h"

#include "hittop/util/is_callable.h"

namespace hittop {
namespace parser {

template <typename T> class ParseResult {
public:
  ParseResult() : error_(ParseError::NONE) {}

  /* implicit */ ParseResult(T value)
      : value_(value), error_(ParseError::NONE) {}

  ParseResult(T value, ParseError err) : value_(value), error_(err) {}

  const T &get() const { return value_; }

  T &&consume() { return std::move(value_); }

  ParseError error() const { return error_; }

  bool ok() const { return error_ == ParseError::NONE; }

private:
  T value_;
  ParseError error_;
};

// The form of a Parser class.  There is no generic implementation of Parser,
// only partial and full specializations that define how to parse specific
// grammars.
template <typename Grammar> class Parser;

// Optimizations are implemented as partial or full template specializations of
// this template.  By default, all parsers are unoptimized.
//
template <typename T> struct OptimizedParser : Parser<T> {};

// Passed to visitors during a parse.
template <typename Grammar, typename Range> struct ParserRunner {
  using Iterator = decltype(std::begin(std::declval<Range>()));
  using OutputRangeType = boost::iterator_range<Iterator>;
  using ResultType = ParseResult<OutputRangeType>;

public:
  explicit ParserRunner(const Range &input) : input_(input) {}

  template <typename... Args> ResultType operator()(Args &&... args);

  ParseResult<Iterator> &&consume() {
    if (!called_) {
      operator()();
    }
    return std::move(result_);
  }

private:
  const Range &input_;
  bool called_ = false;
  ParseResult<Iterator> result_;
};

// Convenience wrapper around defining a new Parser object and invoking it on
// the given input range.  Allows the invocation operator defined on
// Parser<Grammer> to be non-const, as the parser instance created within this
// function is itself non-const.
template <typename Grammar, typename Range, typename Visitor,
          typename = std::enable_if_t<util::IsCallable<
              Visitor, Grammar, ParserRunner<Grammar, const Range &>>::value>,
          typename... Args>
auto Parse(const Range &input, Visitor &&visitor, Args &&... args)
    -> decltype(std::declval<Parser<Grammar>>()(input)) {
  ParserRunner<Grammar, Range> run_parser{input};
  std::forward<Visitor>(visitor)(Grammar{}, run_parser);
  return run_parser.consume();
}

template <typename Grammar, typename Range, typename... Args>
auto Parse(const Range &input, Args &&... args)
    -> decltype(std::declval<Parser<Grammar>>()(input)) {
  Parser<Grammar> parser;
  return parser(input, std::forward<Args>(args)...);
}

// If there are no visitors to match sub-rules of this Grammar, then we can
// transparently apply optimizations, including ones which re-write the Grammar.
template <typename Grammar, typename Range>
auto Parse(const Range &input)
    -> decltype(std::declval<Parser<Grammar>>()(input)) {
  OptimizedParser<Grammar> parser;
  return parser(input);
}

// Convenience function that parses a C string as a literal character range.
template <typename Grammar>
auto Parse(const char *input)
    -> decltype(Parse<Grammar>(boost::as_literal(input))) {
  return Parse<Grammar>(boost::as_literal(input));
}

// Implementation of ParserRunner::operator() - invokes the parser on behalf of
// a visitor.
template <typename Grammar, typename Range>
template <typename... Args>
inline typename ParserRunner<Grammar, Range>::ResultType
ParserRunner<Grammar, Range>::operator()(Args &&... args) {
  called_ = true;
  result_ = Parse<Grammar>(input_, std::forward<Args>(args)...);
  return {boost::make_iterator_range(std::begin(input_), result_.get()),
          result_.error()};
}

} // namespace parser
} // namespace hittop

#define REGISTER_PARSE_RULE(name)                                              \
  names.emplace(std::make_pair(&typeid(name), #name))

#endif // HITTOP_PARSER_PARSER_H
