#ifndef HITTOP_HTTP_PARSE_REQUEST_H
#define HITTOP_HTTP_PARSE_REQUEST_H

#include "hittop/http/grammar.h"
#include "hittop/http/request_parse_visitor.h"
#include "hittop/parser/parser.h"

namespace hittop {
namespace http {

template <typename InputRange, typename RequestType>
auto ParseRequest(const InputRange &input, RequestType *request) {
  return parser::Parse<grammar::Request>(
      input, RequestParseVisitor<RequestType>{request});
}

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_PARSE_REQUEST_H
