#ifndef HITTOP_HTTP_MUTABLE_REQUEST_H
#define HITTOP_HTTP_MUTABLE_REQUEST_H

#include <type_traits>

#include "hittop/concept/macros.h"
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

    CONCEPT_MEMFUN(void, set_http_method(CONCEPT_PARAM(HttpMethod)));

    using mutable_uri_type =
        std::decay_t<decltype(*declval<Derived>().mutable_uri())>;

    uri::MutableUri<mutable_uri_type> uri_check;
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_MUTABLE_REQUEST_H
