// Accept any single character.  Always returns success or incomplete.
//
#ifndef HITTOP_PARSER_ANY_CHAR_H
#define HITTOP_PARSER_ANY_CHAR_H

#include <iterator>

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

struct AnyChar {};

template <> struct IsSingleCharRule<AnyChar> : std::true_type {};

template <> class Parser<AnyChar> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&...) const
      -> ParseResult<decltype(std::begin(input))> {
    auto first = std::begin(input);
    if (first == std::end(input)) {
      return {first, ParseError::INCOMPLETE};
    }
    return std::next(first);
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_ANY_CHAR_H
