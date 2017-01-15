#ifndef HITTOP_PARSER_RFC2616_H
#define HITTOP_PARSER_RFC2616_H

namespace hittop {
namespace parser {
namespace rfc2616 {

using CRLF = Concat<Literal<'\r'>, Literal<'\n'>>;

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

using Reason_Phrase = Repeat<Unless<Either<Literal<'\r'>, //
                                           Literal<'\n'>>,
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

/*
DEFINE_TOKEN(Cache);
DEFINE_TOKEN(Control);
using Cache_Control = Concat<Cache, Literal<'-'>, Control>;

DEFINE_TOKEN(Connection);

DEFINE_TOKEN(Date);

DEFINE_TOKEN(Pragma);

DEFINE_TOKEN(Trailer);

DEFINE_TOKEN(Transfer);
DEFINE_TOKEN(Encoding);
using Tranfer_Encoding = Concat<Transfer, Literal<'-'>, Encoding>;

DEFINE_TOKEN(Upgrade);

DEFINE_TOKEN(Via);

DEFINE_TOKEN(Warning);
*/

using general_header = Either<Cache_Control,     //
                              Connection,        //
                              Date,              //
                              Pragma,            //
                              Trailer,           //
                              Transfer_Encoding, //
                              Upgrade,           //
                              Via,               //
                              Warning>;

using Request_URI = Either<Literal<'*'>, absoluteURI, abs_path, authority>;

using Request_Line = Concat<Method, SP, Request_URI, SP, HTTP_Version, CRLF>;

using Request = Concat<Request_Line,                        //
                       Repeat<Concat<Either<general_header, //
                                            request_header, //
                                            entity_header>, //
                                     CRLF>>,                //
                       CRLF,                                //
                       Opt<message_body>>;

using message_body = Success; // This part of the grammar isn't context free

using field_value = Repeat<Either<field_content, LWS>>;

using field_name = token;

using message_header = Seq<field_name, Literal<':'>, Opt<field_value>>;

using start_line = Either<Request_Line, Status_Line>;

using generic_message = Concat<start_line,                        //
                               Repeat<Seq<message_header, CRLF>>, //
                               CRLF, Opt<message_body>>;

using HTTP_message = Either<Request, Response>;

} // namespace rfc2616

using namespace http_1_1 = rfc2616;

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_RFC2616_H
