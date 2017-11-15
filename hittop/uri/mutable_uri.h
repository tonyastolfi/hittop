#ifndef HITTOP_URI_MUTABLE_URI_H
#define HITTOP_URI_MUTABLE_URI_H

#include <type_traits>

//#include "hittop/uri/mutable_path_segments_type.h"

namespace hittop {
namespace uri {

template <typename Derived> struct MutableUri {
  MutableUri() {
    using part_type = typename Derived::part_type;

    CONCEPT_MEMFUN(void, assign_scheme(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_user(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_host(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_port(CONCEPT_PARAM(unsigned)));
    CONCEPT_MEMFUN(void, assign_authority(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_path(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_query(CONCEPT_PARAM(part_type)));
    CONCEPT_MEMFUN(void, assign_fragment(CONCEPT_PARAM(part_type)));

    // using mutable_path_segments_type =
    //    std::decay_t<decltype(*declval<T>().mutable_path_segments())>;

    // MutablePathSegments<mutable_path_segments_type> path_segments_check;
  }
};

} // namespace uri
} // namespace hittop

#endif // HITTOP_URI_MUTABLE_URI_H
