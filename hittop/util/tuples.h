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

namespace internal {

template <typename F, typename Tuple, std::size_t... I>
constexpr decltype(auto) ApplyImpl(F &&f, Tuple &&t,
                                   std::index_sequence<I...>) {
  return std::forward<F>(f)(std::get<I>(t)...);
}
} // namespace internal

template <typename F, typename Tuple>
constexpr decltype(auto) Apply(F &&f, Tuple &&t) {
  return internal::ApplyImpl(
      std::forward<F>(f), std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

namespace internal {

template <typename T, typename Tuple, std::size_t... I>
constexpr decltype(auto) MakeImpl(Tuple &&t, std::index_sequence<I...>) {
  return T(std::get<I>(t)...);
}
} // namespace internal

template <typename T, typename Tuple> constexpr decltype(auto) Make(Tuple &&t) {
  return internal::MakeImpl<T>(
      std::forward<Tuple>(t),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

namespace internal {

template <typename T, typename Tuple, typename Seq> struct IsMakeableImpl;

template <typename T, typename Tuple, std::size_t... I>
struct IsMakeableImpl<T, Tuple, std::index_sequence<I...>>
    : std::is_constructible<T,
                            decltype(std::get<I>(std::declval<Tuple>()))...> {};
} // namespace internal

template <typename T, typename Tuple>
struct IsMakeable
    : internal::IsMakeableImpl<
          T, Tuple, std::make_index_sequence<
                        std::tuple_size<std::decay_t<Tuple>>::value>> {};

} // namespace tuples
} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_TUPLES_H
