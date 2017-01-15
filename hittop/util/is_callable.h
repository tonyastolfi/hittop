#ifndef HITTOP_UTIL_IS_CALLABLE_H
#define HITTOP_UTIL_IS_CALLABLE_H

#include <cstddef>
#include <type_traits>

namespace hittop {
namespace util {

template <typename T, typename... Args> struct IsCallable {
private:
  template <typename F>
  static std::true_type Impl(decltype(
      std::declval<F>()(std::forward<Args>(std::declval<Args &&>())...)) *);

  template <typename F> static std::false_type Impl(...);

public:
  using type = decltype(Impl<T>(nullptr));
  static const std::size_t value = type::value;
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_IS_CALLABLE_H
