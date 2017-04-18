#ifndef HITTOP_IO_ASYNC_LIMITED_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_LIMITED_BUFFER_STREAM_H

#include "hittop/io/TODO"

namespace hittop {
namespace io {

template <typename AsyncBufferStream>
class AsyncLimitedBufferStream
    : public AsyncConstBufferStream<
          AsyncLimitedBufferStream<AsyncBufferStream>>,
      public AsyncMutableBufferStream<
          AsyncLimitedBufferStream<AsyncBufferStream>> {};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_LIMITED_BUFFER_STREAM_H
