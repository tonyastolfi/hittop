// Interpolate one grammar within instances of another (e.g., to make
// delimiter-separated lists/enumerations)
//
#ifndef HITTOP_PARSER_INTER_H
#define HITTOP_PARSER_INTER_H

#include "hittop/parser/concat.h"
#include "hittop/parser/opt.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/repeat.h"

namespace hittop {
namespace parser {

template <typename Item, typename Delimiter> struct Inter {};

// Define interpolation as: [Item *(Delimiter Item)]
template <typename Item, typename Delimiter>
class Parser<Inter<Item, Delimiter>>
    : public Parser<Opt<Concat<Item, Repeat<Concat<Delimiter, Item>>>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_INTER_H
