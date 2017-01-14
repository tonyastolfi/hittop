// Convenience for at least K copies of a grammar followed by 0 or more.
//
#ifndef HITTOP_PARSER_AT_LEAST_H
#define HITTOP_PARSER_AT_LEAST_H

#include "hittop/parser/concat.h"
#include "hittop/parser/exactly.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/repeat.h"

namespace hittop {
namespace parser {

template <unsigned Count, typename Grammar> struct AtLeast {};

template <unsigned Count, typename Grammar>
class Parser<AtLeast<Count, Grammar>>
    : public Parser<Concat<Exactly<Count, Grammar>, Repeat<Grammar>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_AT_LEAST_H
