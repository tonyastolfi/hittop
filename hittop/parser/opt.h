// Optionally parse a grammar.
//
#ifndef HITTOP_PARSER_OPT_H
#define HITTOP_PARSER_OPT_H

#include <iterator>

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <typename T> struct Opt {};

template <typename T> class Parser<Opt<T>> {
public:
  template <typename Range>
  auto operator()(const Range &in) const -> Fallible<decltype(std::begin(in))> {
    auto result = Parse<T>(in);
    if (!result.error() || result.error() == ParseError::INCOMPLETE) {
      return result;
    }
    // Failure to parse for a reason other than INCOMPLETE is still success;
    //  just return the beginning of the input.
    return std::begin(in);
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_OPT_H
