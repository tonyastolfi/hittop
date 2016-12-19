// Concat: Grammar operator that appends one grammar to another sequentially.
//
#ifndef HITTOP_PARSER_CONCAT_H
#define HITTOP_PARSER_CONCAT_H

#include <iterator>

#include "boost/range/iterator_range_core.hpp"
#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

/*!
 * The Concat operator takes two grammars and "glues" them together
 * sequentially.  It succeeds in parsing input iff the first grammar parses
 * successfully and then the second also parses successfully when run on the
 * unparsed input from the first.
 */
template <typename... Parts> struct Concat {};

/// Parser for an empty sequence == Success
template <> class Parser<Concat<>> {
  template <typename Range>
  auto operator()(const Range &input) const
      -> Fallible<decltype(std::begin(input))> {
    return std::begin(input);
  }
};

using Success = Concat<>;

/// Parser for a sequence of one thing is equivalent to just parsing the thing.
template <typename First> class Parser<Concat<First>> : public Parser<First> {};

/// General recursive case for Parser<Concat<...>>
template <typename First, typename... Rest>
class Parser<Concat<First, Rest...>>
    : public Parser<Concat<First, Concat<Rest...>>> {};

/// Concatenate two grammars; the substantive case of Parse<Concat<...>>.
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

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_CONCAT_H
