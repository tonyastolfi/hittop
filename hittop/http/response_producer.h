#ifndef HITTOP_HTTP_RESPONSE_PRODUCER_H
#define HITTOP_HTTP_RESPONSE_PRODUCER_H

#include <type_traits>

#include "hittop/http/null_response_handler.h"

namespace hittop {
namespace http {

template <typename Derived> class ResponseProducer {
public:
  ResponseProducer() {
    using std::declval;
    static_assert(
        std::is_same<void, decltype(declval<Derived>()(
                               declval<NullResponseHandler>()))>::value,
        "ResponseProducer models must be callable with any model of "
        "ResponseHandler.");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_RESPONSE_PRODUCER_H
