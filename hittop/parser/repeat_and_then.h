// DESCRIPTION
//
#ifndef HITTOP_PARSER_REPEAT_AND_THEN_H
#define HITTOP_PARSER_REPEAT_AND_THEN_H

#include "boost/circular_buffer.hpp"
#include "boost/range/iterator_range_core.hpp"

#include "third_party/short_alloc/short_alloc.h"

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

// Parses Repeated as many times as it can, followed by a successful parse of
// Rest.
//
// This is necessary in the case where a terminal grammar fragment matches a
// subset of the strings that a preceding repeated frament matches.  For
// example:
//
//  fragment = *(alnum) alpha
//
// If this is expressed as:
//
//  using fragment = Concat<Repeat<CharFilter<&std::isalnum>>,
//                          CharFilter<&std::isalpha>>;
//
// then it will fail to parse the valid string: "hello" because the first arg of
// Concat will greedily eat up the entire input, forcing the second arg to fail
// because it requires exactly one character.
//
// Therefore the proper way to encode this would be:
//
//  using fragment = RepeatAndThen<CharFilter<&std::isalnum>,
//                                 CharFilter<&std::isalpha>>;
//
template <typename Repeated, typename Rest,
          std::size_t BacktrackMemorySize = 16>
struct RepeatAndThen {};

template <typename Repeated, typename Rest, std::size_t BacktrackMemorySize>
class Parser<RepeatAndThen<Repeated, Rest, BacktrackMemorySize>> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&... args) const
      -> Fallible<decltype(std::begin(input))> {

    using Iterator = decltype(std::begin(input));
    constexpr std::size_t kSize = sizeof(Iterator) * BacktrackMemorySize * 3;
    using Arena = short_alloc::arena<kSize>;
    using Alloc = short_alloc::short_alloc<Iterator, kSize>;
    using Buffer = boost::circular_buffer<Iterator, Alloc>;

    const auto last = std::end(input);
    Arena arena;
    Buffer prior_success(BacktrackMemorySize, arena);
    prior_success.push_back(std::begin(input));
    for (;;) {
      auto result = Parse<Repeated>(
          boost::make_iterator_range(prior_success.back(), last), args...);
      if (result.error()) {
        if (result.error() == ParseError::INCOMPLETE) {
          return result;
        }
        break;
      }
      // Don't continue if no progress through the input was made.
      if (prior_success.back() == result.get()) {
        break;
      }
      prior_success.push_back(result.consume());
    }

    // Now try to parse the Rest; if we fail, keep back-tracking until we either
    // run out of history or succeed.
    for (;;) {
      auto result = Parse<Rest>(
          boost::make_iterator_range(prior_success.back(), last), args...);
      if (!result.error()) {
        return result;
      }
      if (result.error() == ParseError::INCOMPLETE) {
        return result;
      }
      // Failed, so "unparse" one iteration of Repeated and try again.
      prior_success.pop_back();
      if (prior_success.empty()) {
        return result;
      }
    }
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_REPEAT_AND_THEN_H
