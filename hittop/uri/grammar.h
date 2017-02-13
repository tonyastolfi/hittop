// Uniform Resource Identifiers (URI): Generic Syntax grammar as defined by RFC
// 2396:
//  https://www.ietf.org/rfc/rfc2396.txt
//
#ifndef HITTOP_URI_GRAMMAR_H
#define HITTOP_URI_GRAMMAR_H

#include <cctype>

#include "hittop/parser/at_least.h"
#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/either.h"
#include "hittop/parser/forward_ref.h"
#include "hittop/parser/inter.h"
#include "hittop/parser/literal.h"
#include "hittop/parser/repeat.h"
#include "hittop/parser/repeat_and_then.h"
#include "hittop/parser/unless.h"

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
                              parser::Literal<']'>, parser::Literal<'`'>>;

// uric_no_slash = unreserved | escaped | ";" | "?" | ":" | "@" |
//                 "&" | "=" | "+" | "$" | ","
//
using uric_no_slash = parser::Unless<parser::Literal<'/'>, uric>;

// opaque_part   = uric_no_slash *uric
//
using opaque_part = parser::Concat<uric_no_slash, parser::Repeat<uric>>;

// pchar         = unreserved | escaped |
//                 ":" | "@" | "&" | "=" | "+" | "$" | ","
//
using pchar = parser::Either<unreserved, escaped, parser::Literal<':'>,
                             parser::Literal<'@'>, parser::Literal<'&'>,
                             parser::Literal<'='>, parser::Literal<'+'>,
                             parser::Literal<'$'>, parser::Literal<','>>;

// param         = *pchar
//
struct param_ {
  using type = parser::Repeat<pchar>;
};
using param = parser::ForwardRef<param_>;

// segment       = *pchar *( ";" param )
//
using segment =
    parser::Concat<parser::Repeat<pchar>,
                   parser::Repeat<parser::Concat<parser::Literal<';'>, param>>>;

// path_segments = segment *( "/" segment )
//
using path_segments = parser::Concat<
    segment, parser::Repeat<parser::Concat<parser::Literal<'/'>, segment>>>;

// abs_path      = "/" path_segments
//
using abs_path = parser::Concat<parser::Literal<'/'>, path_segments>;

// path          = [ abs_path | opaque_part ]
//
using path = parser::Opt<parser::Either<abs_path, opaque_part>>;

//   reg_name      = 1*( unreserved | escaped | "$" | "," |
//                       ";" | ":" | "@" | "&" | "=" | "+" )
using reg_name = parser::AtLeast<
    1, parser::Either<
           unreserved, escaped, parser::Literal<'$'>, parser::Literal<','>,
           parser::Literal<';'>, parser::Literal<':'>, parser::Literal<'@'>,
           parser::Literal<'&'>, parser::Literal<'='>, parser::Literal<'+'>>>;

// userinfo      = *( unreserved | escaped |
//                    ";" | ":" | "&" | "=" | "+" | "$" | ","  )
//
using userinfo =
    parser::Repeat<parser::Either<unreserved, escaped, parser::Literal<';'>,
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
        alpha, parser::RepeatAndThen<
                   parser::Either<alphanum, parser::Literal<'-'>>, alphanum>>,
    alpha>;

// hostname      = *( domainlabel "." ) toplabel [ "." ]
//
using hostname = parser::RepeatAndThen<
    parser::Concat<domainlabel, parser::Literal<'.'>>,
    parser::Concat<toplabel, parser::Opt<parser::Literal<'.'>>>>;

// IPv4address   = 1*digit "." 1*digit "." 1*digit "." 1*digit
//
using IPv4Address =
    parser::Concat<parser::AtLeast<1, digit>, parser::Literal<'.'>,
                   parser::AtLeast<1, digit>, parser::Literal<'.'>,
                   parser::AtLeast<1, digit>, parser::Literal<'.'>,
                   parser::AtLeast<1, digit>>;

// port          = *digit
//
using port = parser::Repeat<digit>;

// host          = hostname | IPv4address
//
using host = parser::Either<hostname, IPv4Address>;

// hostport      = host [ ":" port ]
//
using hostport =
    parser::Concat<host,
                   parser::Opt<parser::Concat<parser::Literal<':'>, port>>>;

// userinfo_at   = userinfo "@"
using userinfo_at = parser::Concat<userinfo, parser::Literal<'@'>>;

// server        = [ [ userinfo "@" ] hostport ]
//
using server = parser::Opt<parser::Concat<parser::Opt<userinfo_at>, hostport>>;

// authority     = server | reg_name
//
using authority = parser::Either<reg_name, server>;

// net_path      = "//" authority [abs_path]
//
using net_path = parser::Concat<parser::Literal<'/'>, parser::Literal<'/'>,
                                authority, parser::Opt<abs_path>>;

// query         = *uric
//
struct query_ {
  using type = parser::Repeat<uric>;
};
using query = parser::ForwardRef<query_>;

// fragment      = *uric
//
struct fragment_ {
  using type = parser::Repeat<uric>;
};
using fragment = parser::ForwardRef<fragment_>;

// hier_part     = ( net_path | abs_path ) [ "?" query ]
//
using hier_part =
    parser::Concat<parser::Either<net_path, abs_path>,
                   parser::Opt<parser::Concat<parser::Literal<'?'>, query>>>;

// scheme        = alpha *( alpha | digit | parser::Literal<'+'>,
// parser::Literal<'-'>, "." )

struct scheme_ {
  using type =
      parser::Concat<alpha, parser::Repeat<parser::Either<
                                alpha, digit, parser::Literal<'+'>,
                                parser::Literal<'-'>, parser::Literal<'.'>>>>;
};
using scheme = parser::ForwardRef<scheme_>;

// absoluteURI   = scheme ":" ( hier_part | opaque_part )
using absoluteURI = parser::Concat<scheme, parser::Literal<':'>,
                                   parser::Either<hier_part, opaque_part>>;

// rel_segment   = 1*( unreserved | escaped |
//                      ";" | "@" | "&" | "=" | "+" | "$" | "," )
//
using rel_segment =
    parser::AtLeast<1,
                    parser::Either<unreserved, escaped, parser::Literal<';'>,
                                   parser::Literal<'@'>, parser::Literal<'&'>,
                                   parser::Literal<'='>, parser::Literal<'+'>,
                                   parser::Literal<'$'>, parser::Literal<','>>>;

// rel_path      = rel_segment [ abs_path ]
//
using rel_path = parser::Concat<rel_segment, parser::Opt<abs_path>>;

// relativeURI   = ( net_path | abs_path | rel_path ) [ "?" query ]
//
using relativeURI =
    parser::Concat<parser::Either<net_path, abs_path, rel_path>,
                   parser::Opt<parser::Concat<parser::Literal<'?'>, query>>>;

// URI-reference = [ absoluteURI | relativeURI ] [ "#" fragment ]
//
using URI_reference =
    parser::Concat<parser::Opt<parser::Either<absoluteURI, relativeURI>>,
                   parser::Opt<parser::Concat<parser::Literal<'#'>, fragment>>>;

template <typename Map> void RegisterRuleNames(Map &names) {
  REGISTER_PARSE_RULE(alpha);
  REGISTER_PARSE_RULE(digit);
  REGISTER_PARSE_RULE(alphanum);
  REGISTER_PARSE_RULE(hex);
  REGISTER_PARSE_RULE(control);
  REGISTER_PARSE_RULE(space);
  REGISTER_PARSE_RULE(reserved);
  REGISTER_PARSE_RULE(mark);
  REGISTER_PARSE_RULE(unreserved);
  REGISTER_PARSE_RULE(escaped);
  REGISTER_PARSE_RULE(uric);
  REGISTER_PARSE_RULE(delims);
  REGISTER_PARSE_RULE(unwise);
  REGISTER_PARSE_RULE(uric_no_slash);
  REGISTER_PARSE_RULE(opaque_part);
  REGISTER_PARSE_RULE(pchar);
  REGISTER_PARSE_RULE(param);
  REGISTER_PARSE_RULE(segment);
  REGISTER_PARSE_RULE(path_segments);
  REGISTER_PARSE_RULE(abs_path);
  REGISTER_PARSE_RULE(path);
  REGISTER_PARSE_RULE(reg_name);
  REGISTER_PARSE_RULE(userinfo);
  REGISTER_PARSE_RULE(domainlabel);
  REGISTER_PARSE_RULE(toplabel);
  REGISTER_PARSE_RULE(hostname);
  REGISTER_PARSE_RULE(IPv4Address);
  REGISTER_PARSE_RULE(port);
  REGISTER_PARSE_RULE(host);
  REGISTER_PARSE_RULE(hostport);
  REGISTER_PARSE_RULE(server);
  REGISTER_PARSE_RULE(authority);
  REGISTER_PARSE_RULE(net_path);
  REGISTER_PARSE_RULE(query);
  REGISTER_PARSE_RULE(fragment);
  REGISTER_PARSE_RULE(hier_part);
  REGISTER_PARSE_RULE(scheme);
  REGISTER_PARSE_RULE(absoluteURI);
  REGISTER_PARSE_RULE(rel_segment);
  REGISTER_PARSE_RULE(rel_path);
  REGISTER_PARSE_RULE(relativeURI);
  REGISTER_PARSE_RULE(URI_reference);
}

} // namespace grammar
} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_GRAMMAR_H
