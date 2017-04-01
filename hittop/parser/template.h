// DESCRIPTION
//
#ifndef HITTOP_PARSER_$MODULE_H
#define HITTOP_PARSER_$MODULE_H

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <$params> struct $Module {};

// Always succeed, consuming no input.
template <$params> class Parser<$Module<$args>> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&...) const
      -> ParseResult<decltype(std::begin(input))> {
    return {};
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_$MODULE_H
