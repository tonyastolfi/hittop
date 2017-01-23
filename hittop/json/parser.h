#ifndef HITTOP_JSON_PARSER_H
#define HITTOP_JSON_PARSER_H

#include <tuple>

#include "hittop/parser/parser.h"

#include "hittop/json/grammar.h"
#include "hittop/json/parse_visitor.h"
#include "hittop/json/types.h"

namespace hittop {
namespace json {

template <typename Iterator>
using ParseResult = util::Fallible<std::tuple<Value, Iterator>>;

template <typename Range>
auto ParseValue(const Range &input)
    -> ParseResult<decltype(std::begin(input))> {
  Value output;
  auto result =
      parser::Parse<grammar::Value>(input, ValueParseVisitor{&output});
  return {std::make_tuple(std::move(output), result.consume()), result.error()};
}

} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_PARSER_H
