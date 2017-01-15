#ifndef HITTOP_UTIL_TUPLES_H
#define HITTOP_UTIL_TUPLES_H

#include <cstddef>
#include <tuple>
#include <type_traits>

namespace hittop {
namespace util {
namespace tuples {

// FindFirst<P, Tuple>::type - the first type T in 'Tuple' for which P<T>::value
// is true, or NotFound if none match.
//
// FindFirst<P, Tuple>::index - the position in the tuple where a match is
// found, or the size of the tuple if not found.
//
template <template <typename> class Predicate, typename Tuple> struct FindFirst;

template <template <typename> class Predicate, typename First, typename... Rest>
struct FindFirst<Predicate, std::tuple<First, Rest...>> {
private:
  using find_in_rest = FindFirst<Predicate, std::tuple<Rest...>>;

public:
  using type = typename std::conditional<Predicate<First>::value, First,
                                         typename find_in_rest::type>::type;

  static const std::size_t index =
      Predicate<First>::value ? 0 : 1 + find_in_rest::index;
};

struct NotFound {};

template <template <typename> class Predicate>
struct FindFirst<Predicate, std::tuple<>> {
  using type = NotFound;
  static const std::size_t index = 0;
};

} // namespace tuples

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_TUPLES_H
