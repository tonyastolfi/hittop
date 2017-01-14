// Basic grammar terminal that parses a single character.
//
#ifndef HITTOP_PARSER_LITERAL_H
#define HITTOP_PARSER_LITERAL_H

#include <memory>

#include "hittop/parser/concat.h"
#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

/*!
 * A grammar that accepts only exactly one occurrance of the given character.
 */
template <char... Ch> struct Literal {};

/// Parse implementation for Literal (base case).
template <char Ch> class Parser<Literal<Ch>> {
public:
  template <typename Range>
  auto operator()(const Range &input) const
      -> Fallible<decltype(std::begin(input))> {
    auto next = std::begin(input);
    if (next == std::end(input)) {
      return {std::move(next), ParseError::INCOMPLETE};
    }
    if (*next == Ch) {
      ++next;
      return std::move(next);
    } else {
      return {std::move(next), ParseError::BAD_CHAR};
    }
  }
};

/// Recursive case of Parser<Literal<...>>.
template <char First, char... Rest>
class Parser<Literal<First, Rest...>>
    : public Parser<Concat<Literal<First>, Literal<Rest...>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_LITERAL_H
