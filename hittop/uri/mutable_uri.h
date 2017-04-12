#ifndef HITTOP_URI_MUTABLE_URI_H
#define HITTOP_URI_MUTABLE_URI_H

#include <type_traits>

#include "hittop/uri/mutable_path_segments_type.h"

namespace hittop {
namespace uri {

template <typename T> struct MutableUri {
  MutableUri() {
    using std::declval;
    using range_type = typename T::range_type;

    static_assert(std::is_same<void, decltype(declval<T>().set_scheme(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_user(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_host(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_port(
                                         declval<unsigned>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_authority(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_path(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_query(
                                         declval<range_type>()))>::value,
                  "");

    static_assert(std::is_same<void, decltype(declval<T>().set_fragment(
                                         declval<range_type>()))>::value,
                  "");

    using mutable_path_segments_type =
        std::decay_t<decltype(*declval<T>().mutable_path_segments())>;

    MutablePathSegments<mutable_path_segments_type> path_segments_check;
  }
};

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_MUTABLE_URI_H
