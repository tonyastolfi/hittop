#ifndef HITTOP_PARSER_REPEAT_H
#define HITTOP_PARSER_REPEAT_H

#include "boost/range/iterator_range_core.hpp"

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

// Star operator; repeat the specified grammar greedily as many times as
// possible.  Will always succeed (because parsing 0 times is a valid result)
// or ask for more input, but is not guaranteed to make progress.
template <typename Grammar> struct Repeat {};

template <typename Grammar> class Parser<Repeat<Grammar>> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&... args) const
      -> Fallible<decltype(std::begin(input))> {
    const auto last = std::end(input);
    auto next = std::begin(input);
    for (;;) {
      auto result =
          Parse<Grammar>(boost::make_iterator_range(next, last), args...);
      if (result.error()) {
        // INCOMPLETE is a special case; we always want to pass it through since
        //  it is uncertain whether the parse would have been successful on this
        //  iteration.
        if (result.error() == ParseError::INCOMPLETE) {
          return result;
        }
        break;
      }
      // If we are going to make no progress by running another time
      //  through the loop (we assume Parser implementations are
      //  stateless), then stop here.
      if (next == result.get()) {
        break;
      }
      next = result.consume();
    }
    return next;
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_REPEAT_H
