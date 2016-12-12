#ifndef CASPER_LITERAL_H
#define CASPER_LITERAL_H

#include "casper/parser.h"

namespace casper {

template <char Ch> struct literal {};

template <char Ch> class parser<literal<Ch>> {
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
