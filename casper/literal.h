#ifndef CASPER_LITERAL_H
#define CASPER_LITERAL_H

#include "casper/parser.h"

namespace casper {

template <char Ch> struct literal {};

template <char Ch> class parser<literal<Ch>> {
public:
  template <typename Range>
  auto operator()(const Range& input) const
      -> ParseResult<decltype(std::begin(input))> {
    auto next = std::begin(input);
    if (next == std::end(input)) {
      return {std::move(next), INCOMPLETE};
    }
    if (*next == Ch) {
      ++next;
      return {std::move(next), OK};
    } else {
      return {std::move(next), BAD_CHAR};
    }
  }
};

} // namespace casper

#endif // CASPER_LITERAL_H
