// Convenience for a grammar consisting of exactly K copies of another grammar.
//
#ifndef HITTOP_PARSER_EXACTLY_H
#define HITTOP_PARSER_EXACTLY_H

#include "hittop/parser/concat.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/success.h"

namespace hittop {
namespace parser {

template <unsigned Count, typename Grammar> struct Exactly;

template <typename Grammar>
class Parser<Exactly<0, Grammar>> : public Parser<Success> {};

template <typename Grammar>
class Parser<Exactly<1, Grammar>> : public Parser<Grammar> {};

template <unsigned Count, typename Grammar>
class Parser<Exactly<Count, Grammar>>
    : public Parser<Concat<Grammar, Exactly<Count - 1, Grammar>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_EXACTLY_H
