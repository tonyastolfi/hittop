// Consumes no input, always fails.
//
#ifndef HITTOP_PARSER_FAILURE_H
#define HITTOP_PARSER_FAILURE_H

#include <iterator>

#include "hittop/parser/parse_error.h"
#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

struct Failure {};

// Always fail, consuming no input.
template <> class Parser<Failure> {
public:
  template <typename Range, typename... Args>
  auto operator()(const Range &input, Args &&...) const
      -> Fallible<decltype(std::begin(input))> {
    return {std::begin(input), ParseError::UNKNOWN};
  }
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_FAILURE_H
