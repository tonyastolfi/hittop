// Utilities for producing in-place factories that inject an allocator into
// the construction args.
//
#ifndef HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H
#define HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H

#include <string>
#include <type_traits>

#include "boost/range/iterator_range.hpp"

#include "hittop/util/saved_in_place_factory.h"
#include "hittop/util/tuples.h"

namespace hittop {
namespace util {

namespace internal {
template <template <typename> class Alloc, typename AllocArgsTuple>
class InPlaceFactoryBuilderBase {
public:
  template <typename T> using Allocator = Alloc<T>;

  template <typename... A>
  explicit InPlaceFactoryBuilderBase(A &&... a)
      : alloc_args_(std::forward<A>(a)...) {}

protected:
  AllocArgsTuple alloc_args_;
};
} // internal

template <typename T, template <typename> class Alloc, typename AllocArgsTuple>
struct TypedAllocFactoryBuilder
    : public internal::InPlaceFactoryBuilderBase<Alloc, AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      Alloc, AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename... Args> auto in_place(Args &&... args) {
    return util::SavedInPlace<T>(std::cref(std::forward<Args>(args))...,
                                 tuples::Make<Alloc<T>>(this->alloc_args_));
  }
};

template <typename Iterator, template <typename> class Alloc,
          typename AllocArgsTuple>
class TypedAllocFactoryBuilder<boost::iterator_range<Iterator>, Alloc,
                               AllocArgsTuple>
    : public internal::InPlaceFactoryBuilderBase<Alloc, AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      Alloc, AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename... Args> auto in_place(Args &&... args) {
    return boost::in_place<boost::iterator_range<Iterator>>(
        std::forward<Args>(args)...);
  }
};

template <template <typename> class Alloc, typename AllocArgsTuple>
struct TypedAllocFactoryBuilder<std::string, Alloc, AllocArgsTuple>
    : public internal::InPlaceFactoryBuilderBase<Alloc, AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      Alloc, AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename... Args> auto in_place(Args &&... args) {
    return boost::in_place<std::string>(std::forward<Args>(args)...);
  }
};

template <template <typename> class Alloc, typename AllocArgsTuple>
class InPlaceFactoryBuilder
    : public internal::InPlaceFactoryBuilderBase<Alloc, AllocArgsTuple> {
public:
  using internal::InPlaceFactoryBuilderBase<
      Alloc, AllocArgsTuple>::InPlaceFactoryBuilderBase;

  template <typename T, typename... Args> auto in_place(Args &&... args) {
    using AllocArgsTupleRef =
        typename std::add_lvalue_reference<AllocArgsTuple>::type;

    TypedAllocFactoryBuilder<T, Alloc, AllocArgsTupleRef> typed_builder(
        this->alloc_args_);
    return typed_builder.in_place(std::forward<Args>(args)...);
  }
};

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_IN_PLACE_ALLOC_FACTORY_H
