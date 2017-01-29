// Concatenates zero or more parts, interjecting a delimiter between each one.
//
#ifndef HITTOP_PARSER_IMPLIED_DELIM_H
#define HITTOP_PARSER_IMPLIED_DELIM_H

#include "hittop/parser/concat.h"
#include "hittop/parser/parser.h"
#include "hittop/parser/success.h"

namespace hittop {
namespace parser {

template <typename Delim, typename... Parts> struct ImpliedDelim {};

template <typename Delim, typename First, typename... Rest>
class Parser<ImpliedDelim<Delim, First, Rest...>>
    : public Parser<Concat<First, Delim, ImpliedDelim<Delim, Rest...>>> {};

template <typename Delim>
class Parser<ImpliedDelim<Delim>> : public Parser<Success> {};

template <typename Delim, typename Singleton>
class Parser<ImpliedDelim<Delim, Singleton>> : public Parser<Singleton> {};

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_IMPLIED_DELIM_H
