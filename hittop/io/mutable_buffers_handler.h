#ifndef MUTABLE_BUFFERS_HANDLER_H
#define MUTABLE_BUFFERS_HANDLER_H

#include <functional>
#include <vector>

#include "hittop/io/types.h"

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

// MutableBufferHandler Concept
//
using MutableBuffersHandler = std::function<void(
    const error_code &, cosnt std::vector<mutable_buffer> &)>;

} // namespace io
} // namespace hitttop


#endif // MUTABLE_BUFFERS_HANDLER_H
