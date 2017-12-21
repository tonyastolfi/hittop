#ifndef HITTOP_IO_EMPTY_CONST_BUFFER_STREAM_H
#define HITTOP_IO_EMPTY_CONST_BUFFER_STREAM_H

#include <assert.h>

#include <limits>

#include "boost/asio/error.hpp"

#include "hittop/io/async_const_buffer_stream.h"

namespace hittop {
namespace io {

class EmptyConstBufferStream
    : public AsyncConstBufferStream<EmptyConstBufferStream> {
public:
  using const_buffers_type = std::array<boost::asio::const_buffer, 0>;

  std::size_t size() const { return 0; }

  std::size_t max_size() const {
    return std::numeric_limits<std::size_t>::max();
  }

  template <typename Handler>
  void async_fetch(std::size_t min_count, Handler &&handler) const {
    if (min_count == 0) {
      std::forward<Handler>(handler)(error_code{}, const_buffers_type{});
    } else {
      std::forward<Handler>(handler)(boost::asio::error::eof,
                                     const_buffers_type{});
    }
  }

  void consume(std::size_t count) const { assert(count == 0); }

  void close_for_read() const {}
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_EMPTY_CONST_BUFFER_STREAM_H
