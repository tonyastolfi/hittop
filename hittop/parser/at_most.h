// Convenience for at most K copies of a grammar.
//
#ifndef HITTOP_PARSER_AT_MOST_H
#define HITTOP_PARSER_AT_MOST_H

#include "hittop/parser/at_most.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/opt.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/success.h"

namespace hittop {
namespace parser {

template <unsigned Count, typename Grammar> struct AtMost {};

// AtMost<0, T> always succeeds consuming no input.
template <typename Grammar>
class Parser<AtMost<0, Grammar>> : public Parser<Success> {};

// AtMost<1, T> is just the same as Opt<T>.
template <typename Grammar>
class Parser<AtMost<1, Grammar>> : public Parser<Opt<Grammar>> {};

// [x [x [x ...]]]
// |<---- k ---->|
template <unsigned Count, typename Grammar>
class Parser<AtMost<Count, Grammar>>
    : public Parser<Opt<Concat<Grammar, Opt<AtMost<Count - 1, Grammar>>>>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_AT_MOST_H
