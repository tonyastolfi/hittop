#ifndef HITTOP_UTIL_TUPLES_H
#define HITTOP_UTIL_TUPLES_H

#include <tuple>
#include <type_traits>

namespace hittop {
namespace util {
namespace tuples {

template <template <typename> class Predicate, typename Tuple> struct FindFirst;

template <template <typename> class Predicate, typename First, typename... Rest>
struct FindFirst<Predicate, std::tuple<First, Rest...>> {
  using type = typename std::conditional<
      Predicate<First>::type::value, First,
      typename FindFirst<Predicate, std::tuple<Rest...>>::type>::type;
};

struct NotFound {};

template <template <typename> class Predicate>
struct FindFirst<Predicate, std::tuple<>> {
  using type = NotFound;
};

} // namespace tuples

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_TUPLES_H
