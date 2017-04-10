#ifndef HITTOP_HTTP_HTTP_METHOD_H
#define HITTOP_HTTP_HTTP_METHOD_H

namespace hittop {
namespace http {

enum struct HttpMethod {
  UNKNOWN,
  CONNECT,
  DELETE,
  GET,
  HEAD,
  POST,
  PUT,
  TRACE
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_HTTP_METHOD_H
