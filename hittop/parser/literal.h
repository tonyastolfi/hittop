// Basic grammar terminal that parses a single character.
//
#ifndef HITTOP_PARSER_LITERAL_H
#define HITTOP_PARSER_LITERAL_H

#include <algorithm>

#include "hittop/parser/concat.h"
#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

/*!
 * A grammar that accepts only exactly one occurrance of the given character.
 */
template <char... Ch> struct Literal {};

/// Parse implementation for Literal (base case).
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

/// Recursive case of Parser<Literal<...>>.
template <char First, char... Rest>
class Parser<Literal<First, Rest...>>
    : public Parser<Concat<Literal<First>, Literal<Rest...>>> {};

template <typename Base> struct Token : Base {};

#define DEFINE_TOKEN(name)                                                     \
  struct name##_base_##__LINE__ {                                              \
    static constexpr const char *get() { return #name; }                       \
    template <::std::size_t N>                                                 \
    static constexpr ::std::size_t size(const char (&s)[N]) {                  \
      return N - 1;                                                            \
    }                                                                          \
    static constexpr ::std::size_t size() { return size(#name); }              \
  };                                                                           \
  using name = ::casper::Token<name##_base_##__LINE__>;

template <typename T> class Parser<Token<T>> {
public:
  template <typename Range>
  auto operator()(const Range &input) -> Fallible<decltype(std::begin(input))> {
    auto input_size = boost::size(input);
    auto n = std::min(input_size, T::size());
    const auto first = std::begin(input);
    const auto last = std::next(first, n);
    auto diff_at = std::mismatch(first, last, T::get()).first;
    if (diff_at == last) {
      if (input_size < T::size()) {
        return {std::move(diff_at), ParseError::INCOMPLETE};
      } else {
        return std::move(diff_at);
      }
    } else {
      return {std::move(diff_at), ParseError::BAD_CHAR};
    }
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_LITERAL_H
