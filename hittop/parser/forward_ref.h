// DESCRIPTION
//
#ifndef HITTOP_PARSER_FORWARD_REF_H
#define HITTOP_PARSER_FORWARD_REF_H

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

#endif // HITTOP_PARSER_FORWARD_REF_H
