#ifndef HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H

#include <assert.h>
#include <cstdint>
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

    state_type observed = state_.load(std::memory_order_acquire);
    for (;;) {
      assert(!(observed & kWriterIsBlocked));
      if (observed & kClosedForWrite) {
        // Tried to write after closing for write.
        handler(error::shut_down, {});
      } else if (observed & kClosedForRead) {
        // Tried to write, but the other side has shut down.
        handler(error::broken_pipe, {});
      } else if (write_in_progress_) {
        // There is already a write operation in progress.
        handler(error::already_started, {});
      } else if (buffer_.space() >= minimum_size) {
        // Success!
        write_in_progress_ = true;
        handler({}, buffer_.prepare());
      } else if (observed & kReaderIsBlocked) {
        // Deadlock detected; fail the prepare.
        handler(error::would_block, {});
      } else {
        // Not enough space in the buffer; wait for data to be read.
        write_in_progress_ = true;
        prepare_handler_ = handler;
        minimum_to_prepare_ = minimum_size;
        const state_type updated = inc_writer_seq(observed) | kWriterIsBlocked;
        if (!state_.compare_exchange_weak(observed, updated,
                                          std::memory_order_acq_rel)) {
          write_in_progress_ = false;
          prepare_handler_ = nullptr;
          continue;
        }
      }
      break;
    }
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

  std::size_t max_size() const { return buffer_.max_size(); }

  std::size_t size() const { return buffer_.size(); }

  void async_fetch(std::size_t minimum_size, const FetchHandler &handler) {
    namespace error = boost::asio::error;

    if (minimum_size > max_size()) {
      // The requested size is larger than the buffer.
      handler(error::invalid_argument, {});
      return;
    }

    state_type observed = state_.load(std::memory_order_acquire);
    for (;;) {
      assert(!(observed & kReaderIsBlocked));
      if (observed & kClosedForRead) {
        // Tried to read after closing for read.
        handler(error::shut_down, {});
      } else if (read_in_progress_) {
        // There is already a fetch operation in progress.
        handler(error::already_started, {});
      } else if (buffer_.size() >= minimum_size) {
        // Success!
        read_in_progress_ = true;
        handler({}, buffer_.data());
      } else if (observed & kClosedForWrite) {
        // Not enough data and closed for write; this operation can never
        // succeed.
        handler(error::eof, {});
      } else if (observed & kWriterIsBlocked) {
        // Deadlock detected; fail the fetch.
        handler(error::would_block, {});
      } else {
        // Not enough data; have to wait.
        read_in_progress_ = true;
        fetch_handler_ = handler;
        minimum_to_fetch_ = minimum_size;
        const state_type updated = inc_reader_seq(observed) | kReaderIsBlocked;
        if (!state_.compare_exchange_weak(observed, updated,
                                          std::memory_order_acq_rel)) {
          read_in_progress_ = false;
          fetch_handler_ = nullptr;
          continue;
        }
      }
      break;
    }
  }

  void consume(std::size_t byte_count) {
    assert(read_in_progress_);
    assert(byte_count <= buffer_.size());

    read_in_progress_ = false;
    if (byte_count == 0) {
      return;
    }
    buffer_.consume(byte_count);

    PrepareHandler local_prepare_handler;
    {
      state_type observed = state_.load(std::memory_order_acquire);
      state_type updated;
      do {
        if ((observed & kWriterIsBlocked) && !(observed & kClosedForWrite) &&
            (minimum_to_prepare_ <= buffer_.space())) {
          if (!local_prepare_handler) {
            local_prepare_handler.swap(prepare_handler_);
          }
          updated = observed & ~kWriterIsBlocked;
        } else {
          // Bump the reader update counter to notify the writer that the buffer
          // may have changed.
          updated = inc_reader_seq(observed);

          // We have completely wrapped around without any indication that the
          // writer has seen any updates from us.  This means updating the
          // sequence now may not notify the writer of a change in buffer.  In
          // fact, since we don't know which of our sequence numbers the writer
          // may have seen last, there is no way to safely pick a value to set
          // our sequence count to.  This is an extremely rare condition, so
          // just abort for now.  It is actually recoverable; we can treat it as
          // a stream error.
          //
          if (reader_seq_wrapped(updated)) {
            // Fail! TODO - less dramatically
            std::terminate();
          }
        }
      } while (!state_.compare_exchange_weak(observed, updated,
                                             std::memory_order_acq_rel));
    }
    if (local_prepare_handler) {
      local_prepare_handler({}, buffer_.prepare());
    }
  }

  void close_for_read() {
    namespace error = boost::asio::error;

    state_type observed = state_.load(std::memory_order_acquire);
    if (observed & kClosedForRead) {
      // Already closed or closing.
      return;
    }

    // CAS loop until we can set the closed bit.
    state_type updated;
    do {
      updated = observed | kClosedForRead;
    } while (!state_.compare_exchange_weak(observed, updated,
                                           std::memory_order_release));

    // Once the stream is marked as closed for reading, we can fail any
    // operations that will never complete otherwise.
    //
    if (observed & kReaderIsBlocked) {
      assert(!(observed & kWriterIsBlocked)); // assert no deadlock

      // kReaderIsBlocked *BEFORE* kClosedForRead
      if (fetch_handler_) {
        util::SwapAndInvoke(fetch_handler_, error::broken_pipe);
      }

    } else if (observed & kWriterIsBlocked) {
      assert(!(observed & kReaderIsBlocked)); // assert no deadlock

      // kWriterIsBlocked *BEFORE* kClosedForRead
      if (fetch_handler_) {
        util::SwapAndInvoke(prepare_handler_, error::broken_pipe);
      }
    }
  }

private:
  using state_type = std::uint64_t;

  enum {
    kReaderIsBlockedPos,
    kWriterIsBlockedPos,
    kClosedForReadPos,
    kClosedForWritePos,
    kFlagBits
  };

  static constexpr state_type kFlag = 1;
  static constexpr unsigned kSeqBits = (64 - kFlagBits) / 2;
  static constexpr state_type kSeqMask = (kFlag << kSeqBits) - 1;

  static constexpr state_type kReaderIsBlocked = kFlag << kReaderIsBlockedPos;
  static constexpr state_type kWriterIsBlocked = kFlag << kWriterIsBlockedPos;
  static constexpr state_type kClosedForRead = kFlag << kClosedForReadPos;
  static constexpr state_type kClosedForWrite = kFlag << kClosedForWritePos;

  static constexpr state_type kReaderSeqMask = kSeqMask << kFlagBits;
  static constexpr state_type kReaderInc = kFlag << kFlagBits;
  static constexpr state_type kWriterSeqMask = kSeqMask
                                               << (kFlagBits + kSeqBits);
  static constexpr state_type kWriterInc = kFlag << (kFlagBits + kSeqBits);

  state_type inc_reader_seq(const state_type observed) {
    reader_observes_writer_seq(observed);
    return inc_seq(observed, kReaderSeqMask, kReaderInc);
  }

  state_type inc_writer_seq(const state_type observed) {
    writer_observes_reader_seq(observed);
    return inc_seq(observed, kWriterSeqMask, kWriterInc);
  }

  state_type inc_seq(const state_type observed, const state_type mask,
                     const state_type inc) {
    return (observed & ~mask) | (((observed & mask) + inc) & mask);
  }

  bool reader_seq_wrapped(const state_type updated) {
    return seq_wrapped(updated, last_seq_observed_by_writer_);
  }

  bool writer_seq_wrapped(const state_type updated) {
    return seq_wrapped(updated, last_seq_observed_by_reader_);
  }

  bool seq_wrapped(const state_type updated, const last_observed) {
    return (updated & (kReaderSeqMask | kWriterSeqMask)) == last_observed;
  }

  void writer_observes_reader_seq(const state_type observed) {
    if ((observed & kWriterSeqMask) !=
        (last_seq_observed_by_writer_ & kWriterSeqMask)) {
      last_seq_observed_by_writer_ =
          observed & (kWriterSeqMask | kReaderSeqMask);
    }
  }

  void reader_observes_writer_seq(const state_type observed) {
    if ((observed & kReaderSeqMask) !=
        (last_seq_observed_by_reader_ & kReaderSeqMask)) {
      last_seq_observed_by_reader_ =
          observed & (kWriterSeqMask | kReaderSeqMask);
    }
  }

  CircularBuffer buffer_;
  std::atomic<state_type> state_;
  bool write_in_progress_ = false;
  bool read_in_progress_ = false;
  state_type writer_seq_observed_by_reader_ = 0;
  state_type reader_seq_observed_by_writer_ = 0;

  // Only used by readers when kReaderIsBlocked bit is clear;
  // only used by writers when kReaderIsBlocked bit is set.
  // Do not touch when kClosedForRead bit is true.
  std::size_t minimum_to_fetch_;
  FetchHandler fetch_handler_;

  // Only used by writers when kWriterIsBlocked is false;
  // only used by readers when kWriterIsBlocked is true.
  // Do not touch when kClosedForWrite bit is true.
  std::size_t minimum_to_prepare_;
  PrepareHandler prepare_handler_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_CIRCULAR_BUFFER_STREAM_H
