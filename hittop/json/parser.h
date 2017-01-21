#ifndef HITTOP_JSON_PARSER_H
#define HITTOP_JSON_PARSER_H

#include "hittop/json/grammar.h"
#include "hittop/json/types.h"
#include "hittop/json/visitor.h"

namespace hittop {
namespace json {

template <typename Iterator>
using ParseResult = Fallible<std::tuple<Value, Iterator>>;

tempalte<typename Range> ParseResult ParseValue(const Range &input) {}

} // namespace json
} // namespace hittop

#endif // HITTOP_JSON_PARSER_H
