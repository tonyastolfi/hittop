#ifndef MUTABLE_BUFFERS_HANDLER_H
#define MUTABLE_BUFFERS_HANDLER_H

#include <functional>

#include "boost/container/static_vector.hpp"

#include "hittop/io/mutable_buffer_sequence.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

// MutableBufferHandler Concept
//
using MutableBuffersHandler =
    std::function<void(const error_code &, const MutableBufferSequence &)>;

} // namespace io
} // namespace hitttop

#endif // MUTABLE_BUFFERS_HANDLER_H
