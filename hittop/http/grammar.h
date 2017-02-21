// HTTP/1.1 Grammar as defined by RFC 2616:
//  https://www.ietf.org/rfc/rfc2616.txt
//
#ifndef HITTOP_HTTP_GRAMMAR_H
#define HITTOP_HTTP_GRAMMAR_H

#include <cctype>

#include "hittop/parser/any_char.h"
#include "hittop/parser/at_least.h"
#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/either.h"
#include "hittop/parser/forward_ref.h"
#include "hittop/parser/implied_delim.h"
#include "hittop/parser/literal.h"
#include "hittop/parser/opt.h"
#include "hittop/parser/token.h"
#include "hittop/parser/unless.h"
#include "hittop/uri/grammar.h"

namespace hittop {
namespace http {
namespace grammar {

using OCTET = parser::AnyChar;

using DIGIT = parser::CharFilter<&std::isdigit>;

using SP = parser::Literal<' '>;

using CRLF = parser::Concat<parser::Literal<'\r'>, parser::Literal<'\n'>>;

using LWS =
    parser::Concat<parser::Opt<CRLF>,
                   parser::AtLeast<1, parser::Either<parser::Literal<' '>,
                                                     parser::Literal<'\t'>>>>;

using separators = parser::Either<
    parser::Literal<'('>, parser::Literal<')'>, parser::Literal<'<'>,
    parser::Literal<'>'>, parser::Literal<'@'>, parser::Literal<','>,
    parser::Literal<';'>, parser::Literal<':'>, parser::Literal<'\\'>,
    parser::Literal<'"'>, parser::Literal<'/'>, parser::Literal<'['>,
    parser::Literal<']'>, parser::Literal<'?'>, parser::Literal<'='>,
    parser::Literal<'{'>, parser::Literal<'}'>, parser::Literal<' '>,
    parser::Literal<'\t'>>;

using CTLs = parser::CharFilter<&std::iscntrl>;

using TEXT = parser::Either<LWS, parser::Unless<CTLs, parser::AnyChar>>;

using quoted_pair = parser::Concat<parser::Literal<'\\'>, parser::AnyChar>;

using ctext =
    parser::Unless<parser::Either<parser::Literal<'('>, parser::Literal<')'>>,
                   TEXT>;

struct comment_t;

using comment = parser::ForwardRef<comment_t>;

struct comment_t {
  using type = parser::Concat<
      parser::Literal<'('>,
      parser::Repeat<parser::Either<ctext, quoted_pair, comment>>,
      parser::Literal<')'>>;
};

using qdtext = parser::Unless<parser::Literal<'"'>, parser::AnyChar>;

using quoted_string =
    parser::Concat<parser::Literal<'"'>,
                   parser::Repeat<parser::Either<qdtext, quoted_pair>>,
                   parser::Literal<'"'>>;

using token = parser::AtLeast<
    1, parser::Unless<parser::Either<CTLs, separators>, parser::AnyChar>>;

// Define a helper template for "implied *LWS" (RFC 2616, section 2.2, page 15)
//
template <typename... Parts>
using Glue = parser::ImpliedDelim<parser::Repeat<LWS>, Parts...>;

/* The definition of field-content (RFC/2616, section 4.2, page 32) strikes me
 * as particularly vague:
 *
 *   field-content  = <the OCTETs making up the field-value
 *                    and consisting of either *TEXT or combinations
 *                    of token, separators, and quoted-string>
 *
 * This is my best attempt at a translation to something actually implementable.
 * :-/
 */
using field_content = parser::Either<
    parser::Repeat<TEXT>,
    parser::Repeat<parser::Either<token, separators, quoted_string>>>;

struct field_value_ {
  using type = parser::Repeat<parser::Either<field_content, LWS>>;
};
using field_value = parser::ForwardRef<field_value_>;

struct field_name_ {
  using type = token;
};
using field_name = parser::ForwardRef<field_name_>;

struct message_header_ {
  using type = Glue<field_name, parser::Literal<':'>, parser::Opt<field_value>>;
};
using message_header = parser::ForwardRef<message_header_>;

/*
namespace tokens {

DEFINE_NAMED_TOKEN(Cache_Control, "Cache-Control");
DEFINE_NAMED_TOKEN(Connection, "Connection");
DEFINE_NAMED_TOKEN(Content_Encoding, "Content-Encoding");
DEFINE_NAMED_TOKEN(Content_Language, "Content-Language");
DEFINE_NAMED_TOKEN(Content_Length, "Content-Length");
DEFINE_NAMED_TOKEN(Content_Location, "Content-Location");
DEFINE_NAMED_TOKEN(Content_MD5, "Content-MD5");
DEFINE_NAMED_TOKEN(Content_Range, "Content-Range");
DEFINE_NAMED_TOKEN(Content_Type, "Content-Type");
DEFINE_NAMED_TOKEN(Date, "Date");
DEFINE_NAMED_TOKEN(Pragma, "Pragma");
DEFINE_NAMED_TOKEN(Trailer, "Trailer");
DEFINE_NAMED_TOKEN(Transfer_Encoding, "Transfer-Encoding");
DEFINE_NAMED_TOKEN(Upgrade, "Upgrade");
DEFINE_NAMED_TOKEN(Via, "Via");
DEFINE_NAMED_TOKEN(Warning, "Warning");

} // namespace tokens
*/

using entity_body = parser::Repeat<OCTET>;

/*
using Content_Encoding =
    Glue<tokens::Content_Encoding, parser::Literal<':'>, field_value>;

using Content_Language =
    Glue<tokens::Content_Language, parser::Literal<':'>, field_value>;

using Content_Length = Glue<tokens::Content_Length, parser::Literal<':'>,
                            parser::AtLeast<1, DIGIT>>;
*/

using absoluteURI = uri::grammar::absoluteURI;
using relativeURI = uri::grammar::relativeURI;

/*
using Content_Location = Glue<tokens::Content_Location, parser::Literal<':'>,
                              parser::Either<absoluteURI, relativeURI>>;

using extension_header = message_header;

using entity_header = parser::Either<Allow,            //
                                     Content_Encoding, //
                                     Content_Language, //
                                     Content_Length,   //
                                     Content_Location, //
                                     Content_MD5,      //
                                     Content_Range,    //
                                     Content_Type,     //
                                     Expires,          //
                                     Last_Modified,    //
                                     extension_header>;

using response_header = parser::Either<Accept_Ranges,      //
                                       Age,                //
                                       ETag,               //
                                       Location,           //
                                       Proxy_Authenticate, //
                                       Retry_After,        //
                                       Server,             //
                                       Vary,               //
                                       WWW_Authenticate>;
*/

using Reason_Phrase =
    parser::Repeat<parser::Unless<parser::Either<parser::Literal<'\r'>, //
                                                 parser::Literal<'\n'>>,
                                  TEXT>>;

// HTTP-Version   = "HTTP" "/" 1*DIGIT "." 1*DIGIT
//
DEFINE_NAMED_TOKEN(HTTP_slash, "HTTP/");

using HTTP_Version =
    parser::Concat<HTTP_slash, parser::AtLeast<1, DIGIT>, parser::Literal<'.'>,
                   parser::AtLeast<1, DIGIT>>;

/*
using extension_code = parser::Exactly<3, DIGIT>;

using Status_Code = parser::Either< //
    // TODO(tonyastolfi) - put in all the actual status-codes here?
    extension_code>;
*/
using Status_Code = parser::Exactly<3, DIGIT>;

using Status_Line =
    parser::Concat<HTTP_Version, SP, Status_Code, SP, Reason_Phrase, CRLF>;

using Response = parser::Concat<
    Status_Line, //
    /*
      parser::Repeat<parser::Concat<parser::Either<general_header,  //
                                                   response_header, //
                                                   entity_header>,
                                    CRLF>>,
    */
    parser::Repeat<parser::Concat<message_header, CRLF>>, //
    CRLF>;

/*
using request_header = parser::Either<Accept,              //
                                      Accept_Charset,      //
                                      Accept_Encoding,     //
                                      Accept_Language,     //
                                      Authorization,       //
                                      Expect,              //
                                      From,                //
                                      Host,                //
                                      If_Match,            //
                                      If_Modified_Since,   //
                                      If_None_Match,       //
                                      If_Range,            //
                                      If_Unmodified_Since, //
                                      Max_Forwards,        //
                                      Proxy_Authorization, //
                                      Range,               //
                                      Referer,             //
                                      TE,                  //
                                      User_Agent>;
*/

struct extension_method_ {
  using type = token;
};
using extension_method = parser::ForwardRef<extension_method_>;

DEFINE_TOKEN(OPTIONS);
DEFINE_TOKEN(GET);
DEFINE_TOKEN(HEAD);
DEFINE_TOKEN(POST);
DEFINE_TOKEN(PUT);
DEFINE_TOKEN(DELETE);
DEFINE_TOKEN(TRACE);
DEFINE_TOKEN(CONNECT);

using HttpMethod = parser::Either<OPTIONS, //
                                  GET,     //
                                  HEAD,    //
                                  POST,    //
                                  PUT,     //
                                  DELETE,  //
                                  TRACE,   //
                                  CONNECT  //
                                  >;

using Method = parser::Either<HttpMethod, extension_method>;

/*
using general_header = parser::Either<Cache_Control,     //
                                      Connection,        //
                                      Date,              //
                                      Pragma,            //
                                      Trailer,           //
                                      Transfer_Encoding, //
                                      Upgrade,           //
                                      Via,               //
                                      Warning>;
*/

using abs_path = uri::grammar::abs_path;
using authority = uri::grammar::authority;

using Request_URI =
    parser::Either<parser::Literal<'*'>, absoluteURI, abs_path, authority>;

using Request_Line =
    parser::Concat<Method, SP, Request_URI, SP, HTTP_Version, CRLF>;

using Request = parser::Concat<
    Request_Line, //
    /*
      parser::Repeat<parser::Concat<parser::Either<general_header, //
                                                   request_header, //
                                                   entity_header>, //
                                    CRLF>>,                        //
    */
    parser::Repeat<Glue<message_header, CRLF>>, //
    CRLF>;

/*
using FullRequest = parser::Concat<Request, parser::Opt<message_body>>;

using message_body =
    parser::Success; // This part of the grammar isn't context free
*/

using start_line = parser::Either<Request_Line, Status_Line>;

using generic_message =
    parser::Concat<start_line,                                 //
                   parser::Repeat<Glue<message_header, CRLF>>, //
                   CRLF>;

using HTTP_message = parser::Either<Request, Response>;

template <typename Map> void RegisterRuleNames(Map &names) {
  REGISTER_PARSE_RULE(OCTET);
  REGISTER_PARSE_RULE(DIGIT);
  REGISTER_PARSE_RULE(SP);
  REGISTER_PARSE_RULE(CRLF);
  REGISTER_PARSE_RULE(LWS);
  REGISTER_PARSE_RULE(separators);
  REGISTER_PARSE_RULE(CTLs);
  REGISTER_PARSE_RULE(TEXT);
  REGISTER_PARSE_RULE(quoted_pair);
  REGISTER_PARSE_RULE(ctext);
  REGISTER_PARSE_RULE(comment);
  REGISTER_PARSE_RULE(qdtext);
  REGISTER_PARSE_RULE(quoted_string);
  REGISTER_PARSE_RULE(token);
  REGISTER_PARSE_RULE(field_content);
  REGISTER_PARSE_RULE(field_value);
  REGISTER_PARSE_RULE(field_name);
  REGISTER_PARSE_RULE(message_header);
  REGISTER_PARSE_RULE(entity_body);
  REGISTER_PARSE_RULE(absoluteURI);
  REGISTER_PARSE_RULE(relativeURI);
  REGISTER_PARSE_RULE(Reason_Phrase);
  REGISTER_PARSE_RULE(HTTP_slash);
  REGISTER_PARSE_RULE(HTTP_Version);
  REGISTER_PARSE_RULE(Status_Code);
  REGISTER_PARSE_RULE(Status_Line);
  REGISTER_PARSE_RULE(Response);
  REGISTER_PARSE_RULE(extension_method);
  REGISTER_PARSE_RULE(OPTIONS);
  REGISTER_PARSE_RULE(GET);
  REGISTER_PARSE_RULE(HEAD);
  REGISTER_PARSE_RULE(POST);
  REGISTER_PARSE_RULE(PUT);
  REGISTER_PARSE_RULE(DELETE);
  REGISTER_PARSE_RULE(TRACE);
  REGISTER_PARSE_RULE(CONNECT);
  REGISTER_PARSE_RULE(HttpMethod);
  REGISTER_PARSE_RULE(Method);
  REGISTER_PARSE_RULE(abs_path);
  REGISTER_PARSE_RULE(authority);
  REGISTER_PARSE_RULE(Request_URI);
  REGISTER_PARSE_RULE(Request_Line);
  REGISTER_PARSE_RULE(Request);
  REGISTER_PARSE_RULE(start_line);
  REGISTER_PARSE_RULE(generic_message);
  REGISTER_PARSE_RULE(HTTP_message);
}

} // namespace grammar
} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_GRAMMAR_H
