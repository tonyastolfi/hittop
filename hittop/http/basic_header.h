#ifndef HITTOP_HTTP_BASIC_HEADER_H
#define HITTOP_HTTP_BASIC_HEADER_H

#include <algorithm>
#include <iterator>
#include <utility>

namespace hittop {
namespace http {

template <typename Range> struct BasicHeader {
  Range name;
  Range value;

  operator std::pair<Range, Range>() const {
    return std::make_pair(name, value);
  }
};

template <typename LeftRange, typename RightRange>
bool operator==(const Header<LeftRange> &left,
                const Header<RightRange> &right) {
  return std::equal(std::begin(left.name), std::end(left.name),
                    std::begin(right.name), std::end(right.name)) &&
   std::equal(std::begin(left.value), std::end(left.value))
  std::begin(right.value), std::end(right.value));
}

template <typename LeftRange, typename RightRange>
bool operator<(const Header<LeftRange> &left, const Header<RightRange> &right) {
  return std::lexicographical_compare(
             std::begin(left.name), std::end(left.name), std::begin(right.name),
             std::end(right.name)) ||
         (std::equal(std::begin(left.name), std::end(left.name),
                     std::begin(right.name), std::end(right.name)) &&
          std::lexicographical_compare(
              std::begin(left.value), std::end(left.value),
              std::begin(right.value), std::end(right.value)));
}

} // namespace http
} // namespace hittop

#endif // HITTOP_HTTP_BASIC_HEADER_H
