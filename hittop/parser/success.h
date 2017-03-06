// Consumes no input, always succeeds
//
#ifndef HITTOP_PARSER_SUCCESS_H
#define HITTOP_PARSER_SUCCESS_H

#include <iterator>

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

struct Success {};

// Always succeed, consuming no input.
template <> class Parser<Success> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&...) const
      -> Fallible<decltype(std::begin(input))> {
    return std::begin(input);
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_SUCCESS_H
