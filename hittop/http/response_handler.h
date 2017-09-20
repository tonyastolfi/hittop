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
        "void "
        "ResponseHandler::set_connection_disposition(ConnectionDisposition) "
        "not defined.");

    static_assert(
        std::is_same<void, decltype(declval<Derived>().set_content_length(
                               declval<std::size_t>()))>::value,
        "void ResponseHandler::set_content_length(std::size_t) not defined.");

    // void continue_response(AsyncConstBufferStream*,
    //                        void Complete(error_code))
    //
    static_assert(std::is_same<void, declval<Derived>().async_validate([](
                                         const io::error_code &,
                                         auto &&continue_response) {})>::value,
                  " void ResponseHandler::async_validate(void "
                  "Handler(error_code, void "
                  "ContinueResponse(AsyncConstBufferStream*, void "
                  "Complete(error_code))))) not defined");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_RESPONSE_HANDLER_H
