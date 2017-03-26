#ifndef HITTIP_IO_CONST_BUFFERS_HANDLER_H
#define HITTIP_IO_CONST_BUFFERS_HANDLER_H

#include <functional>

#include "boost/container/static_vector.hpp"

#include "hittop/io/const_buffer_sequence.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

// ConstBufferHandler Concept
//
using ConstBuffersHandler =
    std::function<void(const error_code &, const ConstBufferSequence &)>;

} // namespace io
} // namespace hitttop

#endif // HITTIP_IO_CONST_BUFFERS_HANDLER_H
