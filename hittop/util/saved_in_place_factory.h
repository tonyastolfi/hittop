#ifndef HITTOP_UTIL_SAVED_IN_PLACE_FACTORY_H
#define HITTOP_UTIL_SAVED_IN_PLACE_FACTORY_H

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "boost/utility/in_place_factory.hpp"
#include "boost/utility/typed_in_place_factory.hpp"

namespace hittop {
namespace util {

template <typename T, typename... Args>
class SavedTypedInPlaceFactory : public boost::typed_in_place_factory_base {
  using Tuple = std::tuple<typename std::decay<Args>::type...>;

public:
  explicit SavedTypedInPlaceFactory(Args &&... args)
      : args_(std::forward<Args>(args)...) {}

  void *apply(void *address) const {
    return apply_impl(
        address, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }

private:
  template <std::size_t... I>
  void *apply_impl(void *address, std::index_sequence<I...>) const {
    return new (address) T(std::get<I>(args_)...);
  }

  Tuple args_;
};

template <typename T, typename... Args> auto SavedInPlace(Args &&... args) {
  return SavedTypedInPlaceFactory<T, Args...>(std::forward<Args>(args)...);
}

template <typename... Args>
class SavedInPlaceFactory : public boost::in_place_factory_base {
  using Tuple = std::tuple<typename std::decay<Args>::type...>;

public:
  explicit SavedInPlaceFactory(Args &&... args)
      : args_(std::forward<Args>(args)...) {}

  template <typename T> void *apply(void *address) const {
    return apply_impl<T>(
        address, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
  }

private:
  template <typename T, std::size_t... I>
  void *apply_impl(void *address, std::index_sequence<I...>) const {
    return new (address) T(std::get<I>(args_)...);
  }

  Tuple args_;
};

template <typename... Args> auto SavedInPlace(Args &&... args) {
  return SavedInPlaceFactory<Args...>(std::forward<Args>(args)...);
}

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_SAVED_IN_PLACE_FACTORY_H
