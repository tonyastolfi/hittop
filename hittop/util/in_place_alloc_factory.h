// Utilities for producing in-place factories that inject an allocator into
// the construction args.
//
// TODO - make this a more generic facility; prepend arbitrary arguments to
//  boost::in_place?  to any polymorphic function?
//
#ifndef HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H
#define HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H

#include <functional>
#include <string>
#include <type_traits>

#include "boost/range/iterator_range.hpp"

#include "hittop/util/saved_in_place_factory.h"
#include "hittop/util/tuples.h"

namespace hittop {
namespace util {

namespace internal {
template <typename AllocArgsTuple> class InPlaceFactoryBuilderBase {
public:
  template <typename... A>
  explicit InPlaceFactoryBuilderBase(A &&... a)
      : alloc_args_(std::forward<A>(a)...) {}

protected:
  AllocArgsTuple alloc_args_;
};

template <typename T> struct HasAllocatorType {
private:
  template <typename U>
  static std::true_type Impl(typename U::allocator_type *);

  template <typename U> static std::false_type Impl(...);

public:
  using type = decltype(Impl<T>(nullptr));
  static const std::size_t value = type::value;
};

template <typename T, typename AllocArgsTuple,
          bool HasAlloc = HasAllocatorType<T>::value>
struct TakesAllocator {
  template <typename... Args> struct WithArgs {
    static const bool value =
        std::is_constructible<T, Args...,
                              typename T::allocator_type &&>::value &&
        util::tuples::IsMakeable<typename T::allocator_type,
                                 AllocArgsTuple>::value;
    using type = std::integral_constant<bool, value>;
  };
};

template <typename T, typename AllocArgsTuple>
struct TakesAllocator<T, AllocArgsTuple, false> {
  template <typename... Args> using WithArgs = std::false_type;
};

template <typename T, typename AllocArgsTuple, typename... Args,
          typename = std::enable_if_t<TakesAllocator<
              T, AllocArgsTuple &&>::template WithArgs<Args...>::value>>
auto InPlaceImpl(AllocArgsTuple &&alloc_args, Args &&... args) {
  return util::SavedInPlace<T>(
      std::cref(args)..., tuples::Make<typename T::allocator_type>(alloc_args));
}

template <typename T, typename AllocArgsTuple, typename... Args,
          typename = std::enable_if_t<!TakesAllocator<
              T, AllocArgsTuple>::template WithArgs<Args...>::value>,
          typename = void>
auto InPlaceImpl(AllocArgsTuple &&, Args &&... args) {
  return boost::in_place(args...);
}

} // internal

template <typename T, typename AllocArgsTuple>
struct TypedAllocFactoryBuilder
    : public internal::InPlaceFactoryBuilderBase<AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename... Args> auto in_place(Args &&... args) {
    return internal::InPlaceImpl<T>(this->alloc_args_,
                                    std::forward<Args>(args)...);
  }
};

template <typename AllocArgsTuple>
class AllocFactoryBuilder
    : public internal::InPlaceFactoryBuilderBase<AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename T, typename... Args> auto in_place(Args &&... args) {
    return internal::InPlaceImpl<T>(this->alloc_args_,
                                    std::forward<Args>(args)...);
  }
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H
