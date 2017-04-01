#ifndef HITTOP_UTIL_HASH_H
#define HITTOP_UTIL_HASH_H

#include <functional>
#include <iterator>
#include <numeric>
#include <type_traits>

namespace hittop {
namespace util {

template <typename Range> struct RangeHash {
  std::size_t operator()(const Range &r) const {
    using Value = std::decay_t<decltype(*std::begin(r))>;
    std::hash<Value> hasher;
    return std::accumulate(std::begin(r), std::end(r), std::size_t{0},
                           [&hasher](std::size_t seed, const Value &value) {
                             return seed ^ (hasher(value) + 0x9e3779b9 +
                                            (seed << 6) + (seed >> 2));
                           });
  }
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_HASH_H
