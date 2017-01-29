#ifndef HITTOP_HTTP_GRAMMAR_H
#define HITTOP_HTTP_GRAMMAR_H

#include "hittop/parser/any_char.h"
#include "hittop/parser/at_least.h"
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

using token = parser::AtLeast<1, Unless<Either<CTLs, separators>, AnyChar>>;

using field_content = ;

using field_value = Repeat<Either<field_content, LWS>>;

using field_name = token;

using message_header = Glue<field_name, parser::Literal<':'>, Opt<field_value>>;

namespace tokens {

DEFINE_NAMED_TOKEN(Cache_Control, "Cache-Control");
DEFINE_NAMED_TOKEN(Content_Encoding, "Content-Encoding");
DEFINE_NAMED_TOKEN(Content_Language, "Content-Language");
DEFINE_NAMED_TOKEN(Content_Length, "Content-Length");
DEFINE_NAMED_TOKEN(Content_Location, "Content-Location");
DEFINE_NAMED_TOKEN(Content_MD5, "Content-MD5");
DEFINE_NAMED_TOKEN(Content_Range, "Content-Range");
DEFINE_NAMED_TOKEN(Content_Type, "Content-Type");
DEFINE_TOKEN(Connection);
DEFINE_TOKEN(Date);
DEFINE_TOKEN(Pragma);
DEFINE_TOKEN(Trailer);
DEFINE_NAMED_TOKEN(Transfer_Encoding, "Transfer-Encoding");
DEFINE_TOKEN(Upgrade);
DEFINE_TOKEN(Via);
DEFINE_TOKEN(Warning);

} // namespace tokens

using entity_body = Repeat<OCTET>;

using message_header = Glue<field_name, parser::Literal<':'>, Opt<field_value>>;

using Content_Encoding = Glue<tokens::Content_Encoding, parser::Literal<':'>, >;

using extension_header = message_header;

using entity_header = Either<Allow,            //
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

using response_header = Either<Accept_Ranges,      //
                               Age,                //
                               ETag,               //
                               Location,           //
                               Proxy_Authenticate, //
                               Retry_After,        //
                               Server,             //
                               Vary,               //
                               WWW_Authenticate>;

using Reason_Phrase = Repeat<Unless<Either<parser::Literal<'\r'>, //
                                           parser::Literal<'\n'>>,
                                    TEXT>>;

using extension_code = Exactly<3, Digit>;

using Status_Code = Either< //
    // TODO(tonyastolfi) - put in all the actual status-codes here?
    extension_code>;

using Status_Line =
    Concat<HTTP_Version, SP, Status_Code, SP, Reason_Phrase, CRLF>;

using Response = Concat<Status_Line,                          //
                        Repeat<Concat<Either<general_header,  //
                                             response_header, //
                                             entity_header>,
                                      CRLF>>, //
                        CRLF,                 //
                        Opt<message_body>>;

using request_header = Either<Accept,              //
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

using Method = Either<OPTIONS, //
                      GET,     //
                      HEAD,    //
                      POST,    //
                      PUT,     //
                      DELETE,  //
                      TRACE,   //
                      CONNECT, //
                      extension_method>;

using general_header = Either<Cache_Control,     //
                              Connection,        //
                              Date,              //
                              Pragma,            //
                              Trailer,           //
                              Transfer_Encoding, //
                              Upgrade,           //
                              Via,               //
                              Warning>;

using Request_URI =
    Either<parser::Literal<'*'>, absoluteURI, abs_path, authority>;

using Request_Line = Concat<Method, SP, Request_URI, SP, HTTP_Version, CRLF>;

using Request = Concat<Request_Line,                        //
                       Repeat<Concat<Either<general_header, //
                                            request_header, //
                                            entity_header>, //
                                     CRLF>>,                //
                       CRLF,                                //
                       Opt<message_body>>;

using message_body = Success; // This part of the grammar isn't context free

using start_line = Either<Request_Line, Status_Line>;

using generic_message = Concat<start_line,                         //
                               Repeat<Glue<message_header, CRLF>>, //
                               CRLF, Opt<message_body>>;

using HTTP_message = Either<Request, Response>;

} // namespace grammar
} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_GRAMMAR_H
