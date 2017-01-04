// Remove whitespace from either side of the given grammar.
//
#ifndef HITTOP_PARSER_TRIM_H
#define HITTOP_PARSER_TRIM_H

#include <cctype>

#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/force.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/repeat.h"

namespace hittop {
namespace parser {

template <typename Grammar> struct Trim {};

template <typename Grammar>
class Parser<Trim<Grammar>>                     //
    : public Parser<                            //
          Concat<                               //
              Repeat<CharFilter<std::isspace>>, //
              Grammar,                          //
              Force<Repeat<CharFilter<std::isspace>>>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_TRIM_H
