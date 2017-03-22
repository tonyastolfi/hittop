// A generic rule for combining single-char parsing rules.  The parser for
// CharClass<Rules...> is implemented as a lookup table calculated for each
// possible character value, for all the specified rules.  This guarantees quick
// parsing for arbitrarily complex grammars, so long as they only parse a single
// character.
//
#ifndef HITTOP_PARSER_CHAR_CLASS_H
#define HITTOP_PARSER_CHAR_CLASS_H

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <typename... Rules> struct CharClass {
  static_assert(TrueForAll<IsSingleCharRule, Rules...>::value,
                "CharClass may only combine single-char parsing rules");
};

template <typename... Rules>
struct IsSingleCharRule<CharClass<Rules...>> : std::true_type {};

namespace internal {

template <typename... Rules> struct CharClassTester;

template <> struct CharClassTester<> {
  bool operator()(int) const { return false; }
};

template <typename First, typename... Rest>
struct CharClassTester<First, Rest...> {
  bool operator()(int ch) const {
    std::array<int, 1> a;
    a[0] = ch;
    Parser<First> parser;
    return !parser(a).error() || CharClassTester<Rest...>{}(ch);
  }
};

} // namespace internel

template <typename... Rules> class Parser<CharClass<Rules...>> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&... args) const
      -> Fallible<decltype(std::begin(input))> {
    static auto lookup = BuildLookup();
    auto next = std::begin(input);
    if (next == std::end(input)) {
      return {std::move(next), ParseError::INCOMPLETE};
    }
    if (lookup[*next]) {
      ++next;
      return std::move(next);
    } else {
      return {std::move(next), ParseError::BAD_CHAR};
    }
  }

private:
  static std::array<bool, 256> BuildLookup() {
    std::array<bool, 256> a;
    a.fill(false);
    internal::CharClassTester<Rules...> test;
    for (int c = 0; c < 256; ++c) {
      a[c] = test(c);
    }
    return a;
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_CHAR_CLASS_H
