#ifndef HITTOP_UTIL_TYPE_TRAITS_H
#define HITTOP_UTIL_TYPE_TRAITS_H

#include <iterator>

namespace hittop {
namespace util {

template <typename Range> struct ConstRangeIterator {
  using type = decltype(std::begin(std::declval<const Range &>()));
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_TYPE_TRAITS_H
