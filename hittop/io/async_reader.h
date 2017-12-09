#ifndef HITTOP_IO_ASYNC_READER_H
#define HITTOP_IO_ASYNC_READER_H

#include "hittop/io/async_transfer_task.h"

namespace hittop {
namespace io {

template <typename Source, typename Sink> class AsyncReader;

template <typename Source, typename Sink>
using AsyncReaderBase =
    AsyncTransferTask<Sink, Source, AsyncReader<Source, Sink>>;

template <typename AsyncReadStreamSource, typename AsyncMutableBufferStreamSink>
class AsyncReader : public AsyncReaderBase<AsyncReadStreamSource,
                                           AsyncMutableBufferStreamSink> {
  using Source = AsyncReadStreamSource;
  using Sink = AsyncMutableBufferStreamSink;

  friend AsyncReaderBase<Source, Sink>;

public:
  template <typename SourceArgs, typename SinkArgs>
  AsyncReader(SourceArgs &&source_args, SinkArgs &&sink_args)
      : AsyncReaderBase<Source, Sink>(std::forward<SinkArgs>(sink_args),
                                      std::forward<SourceArgs>(source_args)) {}

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
