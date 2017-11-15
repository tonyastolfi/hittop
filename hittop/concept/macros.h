#ifndef HITTOP_CONCEPT_MACROS_H
#define HITTOP_CONCEPT_MACROS_H

#include <type_traits>

namespace hittop {
namespace concept {} // namespace concept
} // namespace hittop

#define CONCEPT_PARAM(type) std::declval<type>()

#define CONCEPT_MEMFUN(return_type, expression)                                \
  static_assert(                                                               \
      std::is_same<return_type,                                                \
                   decltype(std::declval<Derived>().expression)>::value,       \
      #expression)

#endif // HITTOP_CONCEPT_MACROS_H
