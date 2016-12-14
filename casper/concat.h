// Concat: Grammar operator that appends one grammar to another sequentially.
//
#ifndef CASPER_CONCAT_H
#define CASPER_CONCAT_H

#include <iterator>

#include "boost/range/iterator_range_core.hpp"
#include "casper/parser.h"

namespace casper {

/*!
 * The Concat operator takes two grammars and "glues" them together
 * sequentially.  It succeeds in parsing input iff the first grammar parses
 * successfully and then the second also parses successfully when run on the
 * unparsed input from the first.
 */
template <typename First, typename Second> struct Concat {};

/// Parse implementation for Concat.
template <typename First, typename Second> class Parser<Concat<First, Second>> {
public:
  template <typename Range>
  auto operator()(const Range &input) const
      -> Fallible<decltype(std::begin(input))> {
    auto first_result = Parse<First>(input);
    if (first_result.error()) {
      return first_result;
    } else {
      auto remaining_input =
          boost::make_iterator_range(first_result.get(), std::end(input));
      return Parse<Second>(std::move(remaining_input));
    }
  }
};

} // namespace casper

#endif // CASPER_CONCAT_H
