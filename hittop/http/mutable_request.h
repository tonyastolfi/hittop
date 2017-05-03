#ifndef HITTOP_HTTP_MUTABLE_REQUEST_H
#define HITTOP_HTTP_MUTABLE_REQUEST_H

#include <type_traits>

#include "hittop/uri/mutable_uri.h"

#include "hittop/http/http_method.h"
#include "hittop/http/mutable_message.h"

namespace hittop {
namespace http {

template <typename Derived>
class MutableRequest : public MutableMessage<Derived> {
public:
  MutableRequest() {
    using std::declval;

    static_assert(
        std::is_same<declval<Derived>().set_method(declval<HttpMethod>()),
                     void>::value,
        "");

    using mutable_uri_type =
        std::decay_t<decltype(*declval<Derived>().mutable_uri())>;

    MutableUri<mutable_uri_type> uri_check;
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_MUTABLE_REQUEST_H
