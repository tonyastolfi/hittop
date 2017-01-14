// Wraps another grammar, forcing parses to succeed.
//
#ifndef HITTOP_PARSER_FORCE_H
#define HITTOP_PARSER_FORCE_H

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <typename Grammar> struct Force {};

template <typename Grammar> class Parser<Force<Grammar>> {
public:
  template <typename Range>
  auto operator()(const Range &input) const
      -> Fallible<decltype(std::begin(input))> {
    // Eat the error_condition of the parse result, forcing success.
    return Parse<Grammar>(input).consume();
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_FORCE_H
