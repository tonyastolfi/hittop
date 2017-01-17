// Accepts a single character based on the result of a predicate.
//
#ifndef HITTOP_PARSER_CHAR_FILTER_H
#define HITTOP_PARSER_CHAR_FILTER_H

#include <cctype>
#include <iterator>
#include <type_traits>

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

// A reference to the type of function that std::isalnum is when called
// (possibly overloaded) with a single character (int) argument.
using CharFilterFunction = decltype(                         //
    static_cast<                                             //
        decltype(std::isalnum(std::declval<int>())) (*)(int) //
        >(std::isalnum));

template <CharFilterFunction F> struct CharFilter {};

template <CharFilterFunction F> class Parser<CharFilter<F>> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&...) const
      -> Fallible<decltype(std::begin(input))> {
    auto first = std::begin(input);
    if (first == std::end(input)) {
      return {first, ParseError::INCOMPLETE};
    }
    if (!F(*first)) {
      return {first, ParseError::BAD_CHAR};
    }
    ++first;
    return std::move(first);
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_CHAR_FILTER_H
