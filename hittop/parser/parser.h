// Defines the generic Parser interface for the Casper parser generator library.
//
#ifndef HITTOP_PARSER_PARSER_H
#define HITTOP_PARSER_PARSER_H

#include "hittop/parser/parse_error.h"

#include "hittop/util/fallible.h"

namespace hittop {
namespace parser {

using util::Fallible;

/// The form of a Parser class.  There is no generic implementation of Parser,
/// only partial and full specializations that define how to parse specific
/// grammars.
template <typename Grammar> class Parser;

/// Convenience wrapper around defining a new Parser object and invoking it on
/// the given input range.  Allows the invocation operator defined on
/// Parser<Grammer> to be non-const, as the parser instance created within this
/// function is itself non-const.
template <typename Grammar, typename Range>
auto Parse(const Range &input)
    -> decltype(std::declval<Parser<Grammar>>()(input)) {
  Parser<Grammar> parser;
  return parser(input);
}

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_PARSER_H
