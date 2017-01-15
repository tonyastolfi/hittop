#ifndef ASYNC_MUTABLE_BUFFER_STREAM_H
#define ASYNC_MUTABLE_BUFFER_STREAM_H

#include "hittop/io/types.h"
#include "hittop/io/const_buffers_handler.h"
#include "hittop/io/mutable_buffers_handler.h"

namespace hittop {
namespace io {

// AsyncMutableBufferStream Concept
//
// Requirements:
//
//  void async_prepare(std::size_t minimum_size, MutableBuffersHandler);
//  std::size_t max_space() const;
//  std::size_t space() const;
//  void commit(std::size_t byte_count);
//

} // namespace io
} // namespace hitttop

#endif // ASYNC_MUTABLE_BUFFER_STREAM_H
