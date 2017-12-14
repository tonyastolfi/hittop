#ifndef HITTOP_UTIL_CHAR_RANGE_H
#define HITTOP_UTIL_CHAR_RANGE_H

#include <algorithm>
#include <utility>

#include "boost/operators.hpp"

namespace hittop {
namespace util {

template <typename Begin, typename End = Begin>
class CharRange : boost::totally_ordered<CharRange<Begin, End>> {
public:
  CharRange() = default;

  template <typename B, typename E>
  CharRange(B &&b, E &&e)
      : begin_(std::forward<B>(b)), end_(std::forward<E>(e)) {}

  const Begin &begin() const { return begin_; }
  const End &end() const { return end_; }

private:
  Begin begin_;
  End end_;
};

template <typename Begin, typename End>
CharRange<std::decay_t<Begin>, std::decay_t<End>> MakeCharRange(Begin &&begin,
                                                                End &&end) {
  return {std::forward<Begin>(begin), std::forward<End>(end)};
}

template <typename Begin, typename End, typename OtherRange>
inline bool operator==(const CharRange<Begin, End> &range, OtherRange &&other) {
  return std::distance(range.begin(), range.end()) ==
             std::distance(std::begin(other), std::end(other)) &&
         std::equal(range.begin(), range.end(), std::begin(other));
}

template <typename Begin, typename End, typename OtherRange>
inline bool operator==(OtherRange &&other, const CharRange<Begin, End> &range) {
  return std::distance(range.begin(), range.end()) ==
             std::distance(std::begin(other), std::end(other)) &&
         std::equal(range.begin(), range.end(), std::begin(other));
}

template <typename Begin1, typename End1, typename Begin2, typename End2>
inline bool operator==(const CharRange<Begin1, End1> &range1,
                       const CharRange<Begin2, End2> &range2) {
  return std::distance(range1.begin(), range1.end()) ==
             std::distance(range2.begin(), range2.end()) &&
         std::equal(range1.begin(), range1.end(), range2.begin());
}

template <typename Begin, typename End, typename OtherRange>
inline bool operator<(const CharRange<Begin, End> &range, OtherRange &&other) {
  return std::lexicographical_compare(range.begin(), range.end(),
                                      std::begin(other), std::end(other));
}

template <typename Begin, typename End, typename OtherRange>
inline bool operator<(OtherRange &&other, const CharRange<Begin, End> &range) {
  return std::lexicographical_comparer(std::begin(other), std::end(other),
                                       range.begin(), range.end());
}

template <typename Begin1, typename End1, typename Begin2, typename End2>
inline bool operator<(const CharRange<Begin1, End1> &range1,
                      const CharRange<Begin2, End2> &range2) {
  return std::lexicographical_comparer(range1.begin(), range1.end(),
                                       range2.begin(), range2.end());
}

// TODO - hash

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_CHAR_RANGE_H
