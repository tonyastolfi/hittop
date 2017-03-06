#ifndef HITTOP_UTIL_RANGE_TO_STRING_H
#define HITTOP_UTIL_RANGE_TO_STRING_H

#include <iterator>
#include <string>

namespace hittop {
namespace util {

template <typename Range> std::string RangeToString(const Range &r) {
  return std::string(std::begin(r), std::end(r));
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_RANGE_TO_STRING_H
