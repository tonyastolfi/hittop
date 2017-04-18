#ifndef HITTOP_IO_ASYNC_WRITER_H
#define HITTOP_IO_ASYNC_WRITER_H

#include "hittop/io/async_transfer_task.h"

namespace hittop {
namespace io {

template <typename SourceConstBufferStream, typename TargetAsyncWriteStream>
class AsyncReader : public AsyncTransferTask<SourceConstBufferStream,
                                             TargetAsyncWriteStream> {
  friend class AsyncTransferTask<SourceConstBufferStream,
                                 TargetAsyncWriteStream>;

public:
  using AsyncTransferTask<SourceConstBufferStream,
                          TargetAsyncWriteStream>::AsyncTransferTask;

private:
  template <typename Handler> void pre_transfer(Handler &&handler) {
    this->buffer_->async_fetch(1, std::forward<Handler>(handler));
  }

  template <typename ConstBufferSequence, typename Handler>
  void transfer(ConstBufferSequence &&buffers, Handler &&handler) {
    this->stream_->async_write_some(std::forward<ConstBufferSequence>(buffers),
                                    std::forward<Handler>(handler));
  }

  void post_transfer(const std::size_t count) { this->buffer_->consume(count); }
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_WRITER_H
