#ifndef HITTOP_HTTP_MUTABLE_HEADERS_H
#define HITTOP_HTTP_MUTABLE_HEADERS_H

#include <type_traits>

namespace hittop {
namespace http {

template <typename Derived> class MutableHeaders {
public:
  MutableHeaders() {
    using std::declval;
    using std::string;

    using value_type = typename Derived::value_type;
    using range_type = typename value_type::range_type;

    static_assert(
        std::is_convertible<BasicHeader<range_type>, value_type>::value, "");

    static_assert(std::is_same<void, decltype(declval<Derived>().emplace_back(
                                         declval<range_type>(),
                                         declval<range_type>()))>::value,
                  "void MutableHeaders::emplace_back(range_type, range_type) "
                  "not defined");

    static_assert(
        std::is_same<void, decltype(declval<Derived>().push_back(
                               declval<BasicHeader<range_type>>()))>::value,
        "void MutableHeaders::push_back(BasicHeader<range_type>) not defined");
  }
};

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_MUTABLE_HEADERS_H
