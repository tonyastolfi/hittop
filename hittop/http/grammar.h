#ifndef HITTOP_HTTP_GRAMMAR_H
#define HITTOP_HTTP_GRAMMAR_H

#include "hittop/parser/any_char.h"
#include "hittop/parser/at_least.h"
#include "hittop/parser/char_filter.h"
#include "hittop/parser/concat.h"
#include "hittop/parser/either.h"
#include "hittop/parser/implied_delim.h"
#include "hittop/parser/literal.h"
#include "hittop/parser/opt.h"
#include "hittop/parser/token.h"
#include "hittop/parser/unless.h"

namespace hittop {
namespace http {
namespace grammar {

using OCTET = parser::AnyChar;

using DIGIT = parser::CharFilter<&std::isdigit>;

using CRLF = parser::Concat<parser::Literal<'\r'>, parser::Literal<'\n'>>;

using LWS = parser::Concat<parser::Opt<CRLF>,
                           parser::AtLeast<1, parser::Either<' ', '\t'>>>;

// Define a helper template for "implied *LWS" (RFC 2616, section 2.2, page 15)
//
template <typename... Parts> using Glue = parser::ImpliedDelim<LWS, Parts...>;

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

using comment =
    parser::Concat<parser::Literal<'('>,
                   parser::Repeat<parser::Either<ctext, quoted_pair, comment>>,
                   parser::Literal<')'>>;

using qdtext = parser::Unless<parser::Literal<'"'>, parser::AnyChar>;

using quoted_string =
    parser::Concat<parser::Literal<'"'>,
                   parser::Repeat<parser::Either<qdtext, quoted_pair>>,
                   parser::Literal<'"'>>;

using token = parser::AtLeast<
    1, parser::Unless<parser::Either<CTLs, separators>, parser::AnyChar>>;

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

using field_value = parser::Repeat<parser::Either<field_content, LWS>>;

using field_name = token;

using message_header =
    Glue<field_name, parser::Literal<':'>, parser::Opt<field_value>>;

namespace token {

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

using entity_body = parser::Repeat<OCTET>;

using message_header = Glue<field_name, parser::Literal<':'>, Opt<field_value>>;

using Content_Encoding =
    Glue<tokens::Content_Encoding, parser::Literal<':'>, field_value>;

using Content_Language =
    Glue<tokens::Content_Language, parser::Literal<':'>, field_value>;

using Content_Length = Glue<tokens::Content_Length, parser::Literal<':'>,
                            parser::AtLeast<1, DIGIT>>;

using Content_Location = Glue<tokens::Content_Location, parser::Literal<':'>,
                              parser::Either<absoluteURI, relativeURI>>;

using extension_header = message_header;

using entity_header = Parser::Either<Allow,            //
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

using response_header = Parser::Either<Accept_Ranges,      //
                                       Age,                //
                                       ETag,               //
                                       Location,           //
                                       Proxy_Authenticate, //
                                       Retry_After,        //
                                       Server,             //
                                       Vary,               //
                                       WWW_Authenticate>;

using Reason_Phrase =
    parser::Repeat<paser::Unless<parser::Either<parser::Literal<'\r'>, //
                                                parser::Literal<'\n'>>,
                                 TEXT>>;

using extension_code = parser::Exactly<3, DIGIT>;

using Status_Code = parser::Either< //
    // TODO(tonyastolfi) - put in all the actual status-codes here?
    extension_code>;

using Status_Line =
    parser::Concat<HTTP_Version, SP, Status_Code, SP, Reason_Phrase, CRLF>;

using Response = parser::Concat<
    Status_Line,                                                  //
    parser::Repeat<parser::Concat<parser::Either<general_header,  //
                                                 response_header, //
                                                 entity_header>,
                                  CRLF>>, //
    CRLF,                                 //
    parser::Opt<message_body>>;

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

using extension_method = token;

DEFINE_TOKEN(OPTIONS);
DEFINE_TOKEN(GET);
DEFINE_TOKEN(HEAD);
DEFINE_TOKEN(POST);
DEFINE_TOKEN(PUT);
DEFINE_TOKEN(DELETE);
DEFINE_TOKEN(TRACE);
DEFINE_TOKEN(CONNECT);

using Method = parser::Either<OPTIONS, //
                              GET,     //
                              HEAD,    //
                              POST,    //
                              PUT,     //
                              DELETE,  //
                              TRACE,   //
                              CONNECT, //
                              extension_method>;

using general_header = parser::Either<Cache_Control,     //
                                      Connection,        //
                                      Date,              //
                                      Pragma,            //
                                      Trailer,           //
                                      Transfer_Encoding, //
                                      Upgrade,           //
                                      Via,               //
                                      Warning>;

using Request_URI =
    parser::Either<parser::Literal<'*'>, absoluteURI, abs_path, authority>;

using Request_Line = Concat<Method, SP, Request_URI, SP, HTTP_Version, CRLF>;

using Request = parser::Concat<
    Request_Line,                                                //
    parser::Repeat<parser::Concat<parser::Either<general_header, //
                                                 request_header, //
                                                 entity_header>, //
                                  CRLF>>,                        //
    CRLF,                                                        //
    parser::Opt<message_body>>;

using message_body =
    parser::Success; // This part of the grammar isn't context free

using start_line = parser::Either<Request_Line, Status_Line>;

using generic_message =
    parser::Concat<start_line,                                 //
                   parser::Repeat<Glue<message_header, CRLF>>, //
                   CRLF, parser::Opt<message_body>>;

using HTTP_message = parser::Either<Request, Response>;

} // namespace grammar
} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_GRAMMAR_H
