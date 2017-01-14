// Shortcut for at least I and at most J (inclusive) copies of a grammar.
//
// For example, Between<2, 4, Literal<'a'>> is equivalent 2, 3, or 4
// copies of the literal 'a'.
//
// Between<N, N, T> is always equivalent to saying Exactly<N, T>.  Just use the
// latter form ;-).
//
// Attempts to use Between to specify a negative reptition count will fail at
// compile time.
//
#ifndef HITTOP_PARSER_BETWEEN_H
#define HITTOP_PARSER_BETWEEN_H

#include "hittop/parser/at_most.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/exactly.h"
#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <unsigned MinCount, unsigned MaxCount, typename Grammar,
          bool IsValid = (MinCount <= MaxCount)>
struct Between {};

template <unsigned MinCount, unsigned MaxCount, typename Grammar>
class Parser<Between<MinCount, MaxCount, Grammar, true>>
    : public Parser<Concat<Exactly<MinCount, Grammar>,
                           AtMost<MaxCount - MinCount, Grammar>>> {
  static_assert(MinCount <= MaxCount,
                "Cannot repeat a grammar a negative number of times.");
};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_BETWEEN_H
