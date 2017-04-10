#ifndef HITTOP_HTTP_RESPONSE_HANDLER_H
#define HITTOP_HTTP_RESPONSE_HANDLER_H

#include <type_traits>

#include "hittop/http/connection_disposition.h"
#include "hittop/http/message_body_handler.h"
#include "hittop/http/mutable_response.h"

namespace hittop {
namespace http {

template <typename Derived>
class ResponseHandler : public MutableResponse<Derived>,
                        public MessageBodyHandler<Derived> {
public:
  ResponseHandler() {
    using std::declval;

    static_assert(
        std::is_same<void,
                     decltype(declval<Derived>().set_connection_disposition(
                         declval<ConnectionDisposition>()))>::value,
        "");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_RESPONSE_HANDLER_H
