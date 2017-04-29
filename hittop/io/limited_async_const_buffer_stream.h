#ifndef HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H
#define HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H

#include "hittop/io/async_const_buffer_stream.h"

namespace hittop {
namespace io {

template <typename WrappedStreamType>
class LimitedAsyncConstBufferStream
    : public AsyncConstBufferStream<
          LimitedAsyncConstBufferStream<WrappedStreamType>> {};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H
