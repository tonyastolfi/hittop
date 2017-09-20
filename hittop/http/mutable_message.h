#ifndef HITTOP_HTTP_MUTABLE_MESSAGE_H
#define HITTOP_HTTP_MUTABLE_MESSAGE_H

#include <type_traits>

#include "hittop/http/http_version.h"
#include "hittop/http/mutable_headers.h"

namespace hittop {
namespace http {

template <typename Derived> class MutableMessage {
public:
  MutableMessage() {
    using std::declval;

    static_assert(
        std::is_same<                                                //
            void,                                                    //
            decltype(                                                //
                declval<Derived>().set_major_version(declval<int>()) //
                )                                                    //
            >::value,
        "MutableMessage models must expose a public member function with the "
        "signature: void set_major_version(int)");

    static_assert(
        std::is_same<                                                //
            void,                                                    //
            decltype(                                                //
                declval<Derived>().set_minor_version(declval<int>()) //
                )                                                    //
            >::value,
        "MutableMessage models must expose a public member function with the "
        "signature: void set_minor_version(int)");

    static_assert(
        std::is_same< //
            void,     //
            decltype( //
                declval<Derived>().set_http_version(
                    declval<const HttpVersion &>()) //
                )                                   //
            >::value,
        "MutableMessage models must expose a public member function with the "
        "signature: void set_http_version(const HTTPVersion&)");

    // TODO - We might be better off just requiring a set_header method.
    using mutable_headers_type = decltype(declval<Derived>().mutable_headers());
    MutableHeaders<std::remove_pointer_t<mutable_headers_type>> headers;
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_MUTABLE_MESSAGE_H
