// Basic grammar terminal that parses a single character.
//
#ifndef CASPER_LITERAL_H
#define CASPER_LITERAL_H

#include "casper/parser.h"

namespace casper {

/*!
 * A grammar that accepts only exactly one occurrance of the given character.
 */
template <char Ch> struct Literal {};

/// Parse implementation for Literal.
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

} // namespace casper

#endif // CASPER_LITERAL_H
