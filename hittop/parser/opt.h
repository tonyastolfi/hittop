// Optionally parse a grammar.
//
#ifndef HITTOP_PARSER_OPT_H
#define HITTOP_PARSER_OPT_H

#include <iterator>

#include "hittop/parser/either.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/success.h"

namespace hittop {
namespace parser {

template <typename T> struct Opt {};

// Optionally parsing a T is the same as either parsing it or just succeeding
// without consuming any input.
template <typename T>
class Parser<Opt<T>> : public Parser<Either<T, Success>> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_OPT_H
