#ifndef HITTOP_IO_ASYNC_READER_H
#define HITTOP_IO_ASYNC_READER_H

#include "hittop/io/async_transfer_task.h"

namespace hittop {
namespace io {

template <typename SourceAsyncReadStream,
          typename TargetAsyncMutableBufferStream>
class AsyncReader : public AsyncTransferTask<TargetAsyncMutableBufferStream,
                                             SourceAsyncReadStream> {
  friend class AsyncTransferTask<TargetAsyncMutableBufferStream,
                                 SourceAsyncReadStream>;

public:
  using AsyncTransferTask<TargetAsyncMutableBufferStream,
                          SourceAsyncReadStream>::AsyncTransferTask;

private:
  template <typename Handler> void pre_transfer(Handler &&handler) {
    this->buffer_->async_prepare(this->buffer_->space(),
                                 std::forward<Handler>(handler));
  }

  template <typename MutableBufferSequence, typename Handler>
  void transfer(MutableBufferSequence &&buffers, Handler &&handler) {
    this->stream_->async_read_some(std::forward<MutableBufferSequence>(buffers),
                                   std::forward<Handler>(handler));
  }

  void post_transfer(const std::size_t count) { this->buffer_->commit(count); }
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_READER_H
