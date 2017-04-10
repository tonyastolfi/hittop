#ifndef HITTOP_HTTP_MUTABLE_RESPONSE_H
#define HITTOP_HTTP_MUTABLE_RESPONSE_H

#include <type_traits>

#include "hittop/http/mutable_message.h"

namespace hittop {
namespace http {

template <typename Derived>
class MutableResponse : public MutableMessage<Derived> {
public:
  MutableResponse() {
    using std::declval;

    static_assert(
        std::is_same<void, decltype(declval<Derived>().set_status_code(
                               declval<int>()))>::value,
        "");

    using range_type = typename Derived::range_type;

    static_assert(
        std::is_same<void, decltype(declval<Derived>().set_reason_phrase(
                               declval<range_type>()))>::value,
        "");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_MUTABLE_RESPONSE_H
