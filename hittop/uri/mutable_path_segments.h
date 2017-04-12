#ifndef HITTOP_URI_MUTABLE_PATH_SEGMENTS_H
#define HITTOP_URI_MUTABLE_PATH_SEGMENTS_H

#include <iterator>
#include <type_traits>

namespace hittop {
namespace uri {

template <typename T> struct MutablePathSegments {
  MutablePathSegments() {
    using std::declval;

    using range_type = typename T::range_type;

    static_assert(std::is_pointer<decltype(declval<T>().emplace_back(
                      std::begin(declval<range_type>()),
                      std::end(declval<range_type>()))) *>::value,
                  "");
  }
};

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_MUTABLE_PATH_SEGMENTS_H
