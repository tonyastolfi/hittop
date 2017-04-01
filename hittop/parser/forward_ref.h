// DESCRIPTION
//
#ifndef HITTOP_PARSER_FORWARD_REF_H
#define HITTOP_PARSER_FORWARD_REF_H

#include "boost/preprocessor/cat.hpp"

#include "hittop/parser/parser.h"

namespace hittop {
namespace parser {

template <typename GrammarMetaFunction> struct ForwardRef {};

template <typename GrammarMetaFunction>
struct IsSingleCharRule<ForwardRef<GrammarMetaFunction>>
    : IsSingleCharRule<typename GrammarMetaFunction::type> {};

template <typename GrammarMetaFunction>
class Parser<ForwardRef<GrammarMetaFunction>>
    : public Parser<typename GrammarMetaFunction::type> {};

} // namespace parser
} // namespace hittop

#define DEF_PARSE_RULE(name, definition)                                       \
  struct BOOST_PP_CAT(name, _) {                                               \
    using type = ::hittop::parser::SingleArgType<void definition>::type;       \
  };                                                                           \
  using name = ::hittop::parser::ForwardRef<BOOST_PP_CAT(name, _)>

#endif // HITTOP_PARSER_FORWARD_REF_H
