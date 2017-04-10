#ifndef HITTOP_HTTP_REQUEST_HANDLER_H
#define HITTOP_HTTP_REQUEST_HANDLER_H

#include <type_traits>

#include "hittop/http/mutable_request.h"

namespace hittop {
namespace http {

template <typename Derived>
class RequestHandler : public MutableRequest<Derived> {
public:
  RequestHandler() {
    using std::declval;

    static_assert(std::is_same<void, declval<Derived>().AsyncValidate([](
                                         const io::error_code &,
                                         auto &&continue_request) {})>::value,
                  "");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_REQUEST_HANDLER_H
