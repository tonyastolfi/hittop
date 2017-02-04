// Uniform Resource Identifiers (URI): Generic Syntax grammar as defined by RFC
// 2396:
//  https://www.ietf.org/rfc/rfc2396.txt
//
#ifndef HITTOP_URI_GRAMMAR_H
#define HITTOP_URI_GRAMMAR_H

#include <cctype>

#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/either.h"
#include "hittop/parser/literal.h"

namespace hittop {
namespace uri {
namespace grammar {

// standard char classes
using alpha = parser::CharFilter<&std::isalpha>;
using digit = parser::CharFilter<&std::isdigit>;
using alphanum = parser::CharFilter<&std::isalnum>;
using hex = parser::CharFilter<&std::isxdigit>;
using control = parser::CharFilter<&std::iscntrl>;

// char aliases
using space = parser::Literal<' '>;

// URI-specific char classes
using reserved = parser::Either<parser::Literal<';'>, parser::Literal<'/'>,
                                parser::Literal<'?'>, parser::Literal<':'>,
                                parser::Literal<'@'>, parser::Literal<'&'>,
                                parser::Literal<'='>, parser::Literal<'+'>,
                                parser::Literal<'$'>, parser::Literal<','>>;

using mark = parser::Either<
    parser::Literal<'-'>, parser::Literal<'_'>, parser::Literal<'.'>,
    parser::Literal<'!'>, parser::Literal<'~'>, parser::Literal<'*'>,
    parser::Literal<'\''>, parser::Literal<'('>, parser::Literal<')'>>;

using unreserved = parser::Either<alphanum, mark>;

using escaped = parser::Concat<parser::Literal<'%'>, hex, hex>;

using uric = parser::Either<reserved, unreserved, escaped>;

using delims = parser::Either<parser::Literal<'<'>, parser::Literal<'>'>,
                              parser::Literal<'#'>, parser::Literal<'%'>,
                              parser::Literal<'"'>>;

using unwise = parser::Either<parser::Literal<'{'>, parser::Literal<'}'>,
                              parser::Literal<'|'>, parser::Literal<'\\'>,
                              parser::Literal<'^'>, parser::Literal<'['>,
                              parser::Literal<'] '>, parser::Literal<'`'>>;

// uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
//                 "&" | "=" | "+" | "$" | ","
//
using uric_no_slash = parser::Unless<parser::Literal<'/'>, uric>;

// opaque_part   = uric_no_slash *uric
//
using opaque_part = parser::Concat<uric_no_slash, parser::Repeat<uric>>;

// abs_path      = "/" path_segments
//
using abs_path = parser::Concat<parser::Literal<'/'>, path_segments>;

// reg_name      = 1*( unreserved | escaped | "$" | "," |
//                     ";" | ":" | "@" | "&" | "=" | "+" )
//
using reg_name = parser::AtLeast<
    1, parser::Either<
           unreserved, escaped, parser::Literal<'$'>, parser::Literal<','>,
           parser::Literal<';'>, parser::Literal<':'>, parser::Literal<'@'>,
           parser::Literal<'&'>, parser::Literal<'='>, parser::Literal<'+'>>>;

// userinfo      = *( unreserved | escaped |
//                    ";" | ":" | "&" | "=" | "+" | "$" | "," )
//
using userinfo =
    parser::Repeat<parser::Either<unreserved, unescaped, parser::Literal<';'>,
                                  parser::Literal<':'>, parser::Literal<'&'>,
                                  parser::Literal<'='>, parser::Literal<'+'>,
                                  parser::Literal<'$'>, parser::Literal<','>>>;

// domainlabel   = alphanum | alphanum *( alphanum | "-" ) alphanum
//
using domainlabel =
    parser::Inter<parser::AtLeast<1, alphanum>, parser::Literal<'-'>>;

// toplabel      = alpha | alpha *( alphanum | "-" ) alphanum
//
using toplabel = parser::Either<
    parser::Concat<
        alpha, parser::Repeat<parser::Either<alphanum, parser::Literal<'-'>>>,
        alphanum>,
    alpha>;

// hostname      = *( domainlabel "." ) toplabel [ "." ]
//
using hostname = parser::Concat<
    parser::Repeat<parser::Concat<domainlabel, parser::Literal<'.'>>>, toplabel,
    parser::Opt<parser::Literal<'.'>>>;

// IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
//

// port          = *digit
//

// host          = hostname | IPv4address
//

// hostport      = host [ ":" port ]
//

// server        = [ [ userinfo "@" ] hostport ]
//
using server =
    parser::Opt<parser::Opt<parser::Concat<userinfo, parser::Literal<'@'>>>,
                hostport>;

// authority     = server | reg_name
//
using authority = parser::Either<reg_name, server>;

// net_path      = "//" authority [abs_path]
//
using net_path = parser::Concat<parser::Literal<'/'>, parser::Literal<'/'>,
                                authority, parser::Opt<abs_path>>;

// hier_part     = ( net_path | abs_path ) [ "?" query ]
//
using hier_part =
    parser::Concat<parser::Either<net_path, abs_path>,
                   parser::Opt<parser::Concat<parser::Literal<'?'>, query>>>;

// scheme        = alpha *( alpha | digit | parser::Literal<'+'>,
// parser::Literal<'-'>, "." )
using scheme = parser::Concat<
    alpha,
    parser::Repeat<parser::Either<alpha, digit, parser::Literal<'+'>,
                                  parser::Literal<'-'>, parser::Literal<'.'>>>>;

// absoluteURI   = scheme ":" ( hier_part | opaque_part )
using absoluteURI = parser::Concat<scheme, parser::Literal<':'>,
                                   parser::Either<hier_part, opaque_part>>;

} // namespace grammar
} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_GRAMMAR_H
