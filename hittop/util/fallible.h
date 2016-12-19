#ifndef HITTOP_UTIL_FALLIBLE_H
#define HITTOP_UTIL_FALLIBLE_H

#include <system_error>

namespace hittop {
namespace util {

/*!
 * An instance of 'T' along with a std::error_condition.
 */
template <typename T> class Fallible {
public:
  Fallible() = default;

  /* implicit */ Fallible(T v) : value_{std::move(v)}, error_{} {}

  Fallible(T v, std::error_condition ec)
      : value_{std::move(v)}, error_{std::move(ec)} {}

  const T &get() const { return value_; }

  T &&consume() { return std::move(value_); }

  const std::error_condition &error() const { return error_; }

private:
  T value_ = {};
  std::error_condition error_;
};

/// Equality compare two Fallible values; they need not be exactly the same
/// type to allow equivalence relations across differing types to be wrapped in
/// Fallible<T>.
template <typename T, typename U>
bool operator==(const Fallible<T> &lhs, const Fallible<U> &rhs) {
  return lhs.get() == rhs.get() && lhs.error() == rhs.error();
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_FALLIBLE_H
