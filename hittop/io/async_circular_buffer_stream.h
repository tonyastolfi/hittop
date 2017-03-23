#ifndef HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H

#include "hittop/io/async_const_buffer_stream.h"
#include "hittop/io/async_mutable_buffer_stream.h"
#include "hittop/io/circular_buffer.h"

namespace hittop {
namespace io {

class AsyncCircularBufferStream
    : public AsyncMutableBufferStream<AsyncCircularBufferStream>,
      public AsyncConstBufferStream<AsyncCircularBufferStream> {
public:
  using FetchHandler =
      std::function<void(const error_code &, const const_buffers_type &)>;

  using PrepareHandler =
      std::function<void(const error_code &, const mutable_buffers_type &)>;

  using const_buffers_type = CircularBuffer::const_buffers_type;

  using mutable_buffers_type = CircularBuffer::mutable_buffers_type;

  explicit AsyncCircularBufferStream(const int size_log_2)
      : buffer_(size_log_2) {}

  AsyncCircularBufferStream(const CircularBuffer &) = delete;
  AsyncCircularBufferStream &operator=(const CircularBuffer &) = delete;

  std::size_t max_space() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return buffer_.max_size();
  }

  std::size_t space() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return buffer_.space();
  }

  void async_prepare(std::size_t minimum_size, FetchHandler handler) {
    std::function<void()> action;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (closed_for_write_ || closed_for_read_) {
        // error: stream is closed
      } else if (fetch_handler_) {
        // error: operation already in progress
      } else if (buffer_.space() < minimum_size) {
        minimum_to_fetch_ = minimum_size;
        fetch_handler_ = std::move(handler);
      } else {
        action = [ buffers = buffer_.prepare(), &handler ]() {
          handler({}, buffers);
        };
      }
    }
    if (action) {
      action();
    }
  }

  void commit(std::size_t byte_count) {
    std::function<void()> action;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      assert(byte_count <= buffer_.space());
      buffer_.commit(byte_count);
      if (fetch_handler_ && buffer_.size() >= minimum_to_fetch_) {
        action = [
          handler = std::move(fetch_handler_), buffers = buffer_.data()
        ]() {
          handler({}, buffers);
        };
      }
    }
    if (action) {
      action();
    }
  }

  void async_fetch(std::size_t minimum_size, FetchHandler handler) {
    std::function<void()> action;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (closed_for_read_) {
        // error: closed
      } else if (fetch_handler_) {
        // error: operation in progress
      } else if (minimum_size > buffer_.size()) {
        if (close_for_write_) {
          // not enough data and closed for write; this operation can never
          // succeed.
          // error: end of stream
        } else {
          // not enough data; have to wait
          minimum_to_fetch_ = minimum_size;
          fetch_handler_ = std::move(handler);
        }
      } else {
        action = [ buffers = buffer_.data(), &handler ]() {
          handler({}, buffers);
        };
      }
    }
    if (action) {
      action();
    }
  }

  void consume(std::size_t byte_count) {
    std::function<void()> action;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      assert(byte_count <= buffer_.size());
      buffer_.consume(byte_count);
      if (prepare_handler_ && minimum_to_prepare_ <= buffer_.space()) {
        action = [
          handler = std::move(prepare_handler_), buffers = buffer_.prepare()
        ]() {
          handler(buffers);
        };
      }
    }
    if (action) {
      action();
    }
  }

private:
  CircularBuffer buffer_;

  std::mutex mutex_;
  bool closed_for_read_ = false;
  bool closed_for_write_ = false;
  std::size_t minimum_to_fetch_;
  std::size_t minimum_to_prepare_;
  FetchHandler fetch_handler_;
  PrepareHandler prepare_handler_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
