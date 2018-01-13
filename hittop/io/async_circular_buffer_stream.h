#ifndef HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H

#include <assert.h>
#include <functional>
#include <mutex>

#include "boost/asio/error.hpp"

#include "hittop/io/async_const_buffer_stream.h"
#include "hittop/io/async_mutable_buffer_stream.h"
#include "hittop/io/circular_buffer.h"

namespace hittop {
namespace io {

// Ideas for how to make concurrent operation more efficient:
//
// 1. Make sure rd/wr heads in CircularBuffer are updated atomically (DONE)
// 2. Separate into reader/writer state in terms of mutability; There are points
// where control is transferred from one to the other, and at those points we
// need an atomic release/acquire pair.  We can make sure actions happen by
// using a technique similar to OrderedActionPair; try to pass off the work on
// the other guy, and then loop until you've succeeded or they have.
// 3. To implement #2, have a single atomically updated state word that is
// broken into flag bits and perhaps smaller integer counts.
//
//

class AsyncCircularBufferStream
    : public AsyncMutableBufferStream<AsyncCircularBufferStream>,
      public AsyncConstBufferStream<AsyncCircularBufferStream> {

  using Action = std::function<void()>;

public:
  using const_buffers_type = CircularBuffer::const_buffers_type;

  using mutable_buffers_type = CircularBuffer::mutable_buffers_type;

  using FetchHandler =
      std::function<void(const error_code &, const const_buffers_type &)>;

  using PrepareHandler =
      std::function<void(const error_code &, const mutable_buffers_type &)>;

  explicit AsyncCircularBufferStream(const int size_log_2 = 12)
      : buffer_(size_log_2) {}

  // Disable copying
  AsyncCircularBufferStream(const CircularBuffer &) = delete;
  AsyncCircularBufferStream &operator=(const CircularBuffer &) = delete;

  ~AsyncCircularBufferStream() {
    close_for_read();
    close_for_write();
  }

  std::size_t max_space() const { return buffer_.max_size(); }

  std::size_t space() const { return buffer_.space(); }

  void async_prepare(std::size_t minimum_size, const PrepareHandler &handler) {
    namespace error = boost::asio::error;

    if (minimum_size > max_space()) {
      // The requested size is larger than the buffer.
      handler(error::invalid_argument, {});
      return;
    }

    state_type observed = state_.load();
    for (;;) {
      if (get_closed_for_write(observed)) {
        // Tried to write after closing for write.
        handler(error::shut_down, {});
        return;
      }
      if (get_closed_for_read(observed)) {
        // Tried to write, but the other side has shut down.
        handler(error::broken_pipe, {});
        return;
      }
      if (get_write_in_progress(observed)) {
        // There is already a write operation in progress.
        handler(error::already_started, {});
        return;
      }
      state_type new_state = set_write_in_progress(observed, true);
      if (buffer_.space() >= minimum_size) {
        // The reader can only increase the available space, so once this is
        // true, we don't need to worry about it becoming untrue (because there
        // should only be a single writer).
        if (state_.compare_exchange_weak(observed, new_state)) {
          // Success!
          handler({}, buffer_.prepare());
          return;
        } // else loop until we get it
      } else {
        // Not enough space in the buffer; wait for data to be read.
        new_state = set_minimum_to_prepare(new_state, minimum_size);
        new_state = set_reader_calls_prepare_handler(new_state, true);
        if (!prepare_handler_) {
          prepare_handler_ = handler; // this is safe because `prepare_handler_`
          // is only ever mutated by the writer TODO -
          // fix this comment
        }
        if (state_.compare_exchange_weak(observed, new_state)) {
          // Nothing else to do, because we've successfully set the
          // reader-calls-prepare-handler bit, someone else will take care of
          // completing the operation.
          return;
        }
        return {};
      }
    }

    with_mutex_locked([&]() -> Action {
      if (closed_for_write_) {
        return [&handler]() { handler(error::shut_down, {}); };
      } else if (closed_for_read_) {
      } else if (write_in_progress_) {
        // There is already a write operation in progress.
        return [&handler]() { handler(error::already_started, {}); };
      } else {
        write_in_progress_ = true;
        if (buffer_.space() < minimum_size) {
          // Not enough space in the buffer; wait for data to be read.
          minimum_to_prepare_ = minimum_size;
          prepare_handler_ = handler;
          return {};
        } else {
          return [ buffers = buffer_.prepare(), &handler ]() {
            handler({}, buffers);
          };
        }
      }
    });
  }

  void commit(const std::size_t byte_count) {
    with_mutex_locked([&]() -> Action {
      assert(byte_count <= buffer_.space());
      assert(write_in_progress_);
      buffer_.commit(byte_count);
      write_in_progress_ = false;
      if (fetch_handler_ && buffer_.size() >= minimum_to_fetch_) {
        auto action = [
          handler = std::move(fetch_handler_), buffers = buffer_.data()
        ]() {
          handler({}, buffers);
        };
        fetch_handler_ = nullptr;
        return std::move(action);
      } else {
        return {};
      }
    });
  }

  void close_for_write() {
    namespace error = boost::asio::error;

    with_mutex_locked([&]() -> Action {
      if (closed_for_write_) {
        return {};
      }
      closed_for_write_ = true;
      if (fetch_handler_) {
        if (prepare_handler_) {
          auto action = [
            prepare_handler = std::move(prepare_handler_),
            fetch_handler = std::move(fetch_handler_)
          ]() {
            prepare_handler(error::shut_down, {});
            fetch_handler(error::eof, {});
          };
          prepare_handler_ = nullptr;
          fetch_handler_ = nullptr;
          read_in_progress_ = write_in_progress_ = false;
          return std::move(action);
        } else {
          auto action = [fetch_handler = std::move(fetch_handler_)]() {
            fetch_handler(error::eof, {});
          };
          fetch_handler_ = nullptr;
          read_in_progress_ = false;
          return std::move(action);
        }
      } else if (prepare_handler_) {
        auto action = [prepare_handler = std::move(prepare_handler_)]() {
          prepare_handler(error::shut_down, {});
        };
        prepare_handler_ = nullptr;
        write_in_progress_ = false;
        return action;
      } else {
        return {};
      }
    });
  }

  std::size_t max_size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return buffer_.max_size();
  }

  std::size_t size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return buffer_.size();
  }

  void async_fetch(std::size_t minimum_size, const FetchHandler &handler) {
    namespace error = boost::asio::error;

    if (minimum_size > max_size()) {
      // The requested size is larger than the buffer.
      handler(error::invalid_argument, {});
      return;
    }

    with_mutex_locked([&]() -> Action {
      if (closed_for_read_) {
        // Tried to read after closing for read.
        return [&handler]() { handler(error::shut_down, {}); };
      } else if (read_in_progress_) {
        // There is already a fetch operation in progress.
        return [&handler]() { handler(error::already_started, {}); };
      } else if (minimum_size > buffer_.size()) {
        if (closed_for_write_) {
          // Not enough data and closed for write; this operation can never
          // succeed.
          return [&handler]() { handler(error::eof, {}); };
        } else {
          // Not enough data; have to wait.
          read_in_progress_ = true;
          minimum_to_fetch_ = minimum_size;
          fetch_handler_ = handler;
          return {};
        }
      } else {
        // Success!
        read_in_progress_ = true;
        return [ buffers = buffer_.data(), &handler ]() {
          handler({}, buffers);
        };
      }
    });
  }

  void consume(std::size_t byte_count) {
    with_mutex_locked([&]() -> Action {
      assert(byte_count <= buffer_.size());
      assert(read_in_progress_);
      buffer_.consume(byte_count);
      read_in_progress_ = false;
      if (prepare_handler_ && minimum_to_prepare_ <= buffer_.space()) {
        auto action = [
          handler = std::move(prepare_handler_), buffers = buffer_.prepare()
        ]() {
          handler({}, buffers);
        };
        prepare_handler_ = nullptr;
        return std::move(action);
      } else {
        return {};
      }
    });
  }

  void close_for_read() {
    namespace error = boost::asio::error;

    with_mutex_locked([&]() -> Action {
      if (closed_for_read_) {
        return {};
      }

      closed_for_read_ = true;
      if (fetch_handler_) {
        if (prepare_handler_) {
          auto action = [
            fetch_handler = std::move(fetch_handler_),
            prepare_handler = std::move(prepare_handler_)
          ]() {
            fetch_handler(error::shut_down, {});
            prepare_handler(error::broken_pipe, {});
          };
          fetch_handler_ = nullptr;
          prepare_handler_ = nullptr;
          return std::move(action);
        } else {
          auto action = [fetch_handler = std::move(fetch_handler_)]() {
            fetch_handler(error::shut_down, {});
          };
          fetch_handler_ = nullptr;
          return std::move(action);
        }
      } else if (prepare_handler_) {
        auto action = [prepare_handler = std::move(prepare_handler_)]() {
          prepare_handler(error::broken_pipe, {});
        };
        prepare_handler_ = nullptr;
        return std::move(action);
      } else {
        return {};
      }
    });
  }

private:
  template <typename F> void with_mutex_locked(F &&f) {
    decltype(f()) action;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      action = std::forward<F>(f)();
    }
    if (action) {
      action();
    }
  }

  CircularBuffer buffer_;
  std::atomic<std::size_t> state_;
  /*
  bool write_in_progress_ = false;
  bool read_in_progress_ = false;
  bool closed_for_read_ = false;
  bool closed_for_write_ = false;
  std::size_t minimum_to_fetch_;
  std::size_t minimum_to_prepare_;
  */
  FetchHandler fetch_handler_;
  PrepareHandler prepare_handler_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
