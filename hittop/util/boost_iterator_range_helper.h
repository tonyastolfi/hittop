#ifndef HITTOP_UTIL_BOOST_ITERATOR_RANGE_HELPER_H
#define HITTOP_UTIL_BOOST_ITERATOR_RANGE_HELPER_H

#include <algorithm>

#include "boost/range/iterator_range.hpp"

#include "hittop/util/hash.h"
#include "hittop/util/range_to_string.h"

namespace hittop {
namespace util {

template <typename LeftRange, typename RightRange>
bool RangeEqual(const LeftRange &left, const RightRange &right) {
  return std::equal(std::begin(left), std::end(left), //
                    std::begin(right), std::end(right));
}

} // namespace util
} // namespace hittop

namespace boost {

template <typename Iterator, typename RightRange>
bool operator==(const iterator_range<Iterator> &left, const RightRange &right) {
  return ::hittop::util::RangeEqual(left, right);
}

template <typename Iterator, typename LeftRange>
bool operator==(const LeftRange &left, const iterator_range<Iterator> &right) {
  return ::hittop::util::RangeEqual(left, right);
}

template <typename Iterator>
std::ostream &operator<<(std::ostream &out, const iterator_range<Iterator> &r) {
  // TODO - make this work for more than just strings :-/
  return out << ::hittop::util::RangeToString(r);
}

} // namespace boost

namespace std {

template <typename Iterator>
struct hash<boost::iterator_range<Iterator>>
    : ::hittop::util::RangeHash<boost::iterator_range<Iterator>> {};

} // namespace std

#endif // HITTOP_UTIL_BOOST_ITERATOR_RANGE_HELPER_H
