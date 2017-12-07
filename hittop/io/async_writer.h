#ifndef HITTOP_IO_ASYNC_WRITER_H
#define HITTOP_IO_ASYNC_WRITER_H

#include "hittop/io/async_transfer_task.h"

namespace hittop {
namespace io {

template <typename Source, typename Sink> class AsyncWriter;

template <typename Source, typename Sink>
using AsyncWriterBase =
    AsyncTransferTask<Source, Sink, AsyncWriter<Source, Sink>>;

template <typename AsyncConstBufferStreamSource, typename AsyncWriteStreamSink>
class AsyncWriter : public AsyncWriterBase<AsyncConstBufferStreamSource,
                                           AsyncWriteStreamSink> {
  using Source = AsyncConstBufferStreamSource;
  using Sink = AsyncWriteStreamSink;

  friend AsyncWriterBase<Source, Sink>;

public:
  using AsyncWriterBase<Source, Sink>::AsyncWriterBase;

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