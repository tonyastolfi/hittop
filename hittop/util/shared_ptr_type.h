#ifndef HITTOP_UTIL_SHARED_PTR_TYPE_H
#define HITTOP_UTIL_SHARED_PTR_TYPE_H

#include <memory>
#include <type_traits>

#include "boost/intrusive_ptr.hpp"
#include "boost/smart_ptr/intrusive_ref_counter.hpp"

namespace hittop {
namespace util {

// Meta-function that returns the most optimal shared pointer type for a given
// type; usually this is just std::shared_ptr<T>, but if T has internal
// reference counting, we may be able to boost::intrusive_ptr<T> or some other
// smart pointer type.  This template should be specialized for types which wish
// to override the default.
//
// \return boost::intrusive_ptr<T> if T derives from
// boost::intrusive_ref_counter<T>, otherwise std::shared_ptr<T>
//
template <typename T> struct SharedPointerType {
  using type = std::conditional_t<
      std::is_assignable<boost::intrusive_ref_counter<T> &, T &>::value,
      boost::intrusive_ptr<T>, std::shared_ptr<T>>;
};

// Convenience alias for typename SharedPointerType<T>::type;
template <typename T> using shared_ptr_t = typename SharedPointerType<T>::type;

} // namespace util
} // namespace hittop

#endif // HITTOP_UTIL_SHARED_PTR_TYPE_H
