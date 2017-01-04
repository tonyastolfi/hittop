#ifndef HITTOP_PARSER_TOKEN_H
#define HITTOP_PARSER_TOKEN_H

#include <algorithm>
#include <cstddef>
#include <iterator>

#include "boost/range/size.hpp"

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <typename Base> struct Token : Base {};

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

#define DEFINE_TOKEN(name) DEFINE_NAMED_TOKEN(name, #name)

#define DEFINE_NAMED_TOKEN(name, token_str)                                    \
  struct name##_base_##__LINE__ {                                              \
    static constexpr const char *get() { return token_str; }                   \
    template <::std::size_t N>                                                 \
    static constexpr ::std::size_t size(const char (&s)[N]) {                  \
      return N - 1;                                                            \
    }                                                                          \
    static constexpr ::std::size_t size() { return size(token_str); }          \
  };                                                                           \
  using name = ::hittop::parser::Token<name##_base_##__LINE__>

#endif // HITTOP_PARSER_TOKEN_H
