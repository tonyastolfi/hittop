#ifndef CASPER_REPEAT_H
#define CASPER_REPEAT_H

#include "boost/range/iterator_range_core.hpp"

namespace casper {

template <typename Grammar, unsigned Min = 0,
          unsigned Max = std::numeric_limits<unsigned>::max()>
struct Repeat {};

template <typename T> using Opt = Repeat<T, 0, 1>;

template <typename Grammar, unsigned Min, unsigned Max>
class Parser<Repeat<Grammar, Min, Max>> {
public:
  template <typename Range>
  auto operator()(const Range &input) const
      -> Fallible<decltype(std::begin(input))> {
    const auto last = std::end(input);
    auto next = std::begin(input);
    for (unsigned count = 0; count < Max; ++count) {
      auto result = Parse<Grammar>(boost::make_iterator_range(next, last));
      if (result.error()) {
        if (count >= Min) {
          break;
        } else {
          return result;
        }
      }
      // If we are going to make no progress by running another time
      //  through the loop (we assume Parser implementations are
      //  stateless).
      if (next == result.get()) {
        break;
      }
      next = result.consume();
    }
    return next;
  }
};

} // namespace casper

#endif // CASPER_REPEAT_H
