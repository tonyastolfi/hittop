#ifndef HITTOP_IO_CONCAT_CONST_BUFFER_STREAM_H
#define HITTOP_IO_CONCAT_CONST_BUFFER_STREAM_H

#include <assert.h>

#include <atomic>
#include <cstddef>

#include "boost/asio/error.hpp"
#include "boost/container/static_vector.hpp"
#include "boost/optional.hpp"
#include "boost/utility/in_place_factory.hpp"

#include "hittop/io/async_const_buffer_stream.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

template <typename First, typename Second, std::size_t kMaxBufferCount = 4>
class ConcatConstBufferStream
    : public AsyncConstBufferStream<ConcatConstBufferStream<First, Second>> {
public:
  using const_buffers_type = //
      boost::container::static_vector<const_buffer, kMaxBufferCount>;

  ConcatConstBufferStream() {}

  std::size_t max_size() const {
    // In general we can't fetch more than the smaller of these two, since if
    // `first_` is full, we can't read past it to its end and into `second_`.
    //
    return std::min(first_.max_size(), second_.max_size());
  }

  std::size_t size() const {
    const std::size_t actual_size =
        first_known_available_ ? *first_known_available_ + second_.size()
                               : first_.size();

    // Its technically possible, if there are some bytes in `second_` and
    // `first_` has a small `max_size()` for instance, for the actual known
    // number of bytes available to exceed the conservative maximum we calculate
    // above; let's just preserve the sanity of callers by clipping to the max.
    //
    return std::min(actual_size, max_size());
  }

  template <typename Handler>
  void async_fetch(std::size_t min_count, Handler &&handler) {
    // Silly edge case.
    if (min_count == 0) {
      handler(error_code{}, const_buffers_type{});
      return;
    }

    // Fail fast if the client asks for more than `max_size()`.
    if (min_count > max_size()) {
      handler(boost::asio::error::message_size, const_buffers_type{});
      return;
    }

    // We have to fetch data serially from `first_` then (if the minimum isn't
    // met) from `second_`.  The exception is when we know that we've consumed
    // the end of `first_` so then we just go on to second.
    //
    if (first_known_available_) {
      if (*first_known_available_ == 0) {
        fetch_from_second(min_count, std::forward<Handler>(handler));
        return;
      }
      if (min_count > *first_known_available_) {
        fetch_from_first_and_second(min_count, std::forward<Handler>(handler));
        return;
      }
    }
    fetch_from_first(min_count, std::forward<Handler>(handler));
  }

  void consume(std::size_t count) {
    if (first_fetched_size_) {
      if (count < *first_fetched_size_) {
        first_.consume(count);
        count = 0;
      } else {
        first_.consume(*first_fetched_size_);
        count -= *first_fetched_size_;
      }
      first_fetched_size_ = boost::none;
    }
    if (second_fetched_size_) {
      assert(count <= *second_fetched_size_);
      second_.consume(count);
      second_fetched_size_ = boost::none;
    }
  }

  void close_for_read() {
    assert(!first_fetched_size_);
    assert(!second_fetched_size_);

    first_.close_for_read();
    second_.close_for_read();
  }

private:
  template <typename Handler>
  void fetch_from_first(const std::size_t min_count, Handler &&handler) {
    // If min_count is past the end of `first_`, then we'll fail with EOF and
    // `first_.size()` will tell us how many bytes we can fetch from the first
    // stream.  Otherwise we'll succeed (barring I/O error) in fetching the
    // required number of bytes.
    first_.async_fetch(
        min_count,
        [ this, captured_handler = std::move(handler),
          min_count ](const error_code &ec,
                      const typename First::const_buffers_type &first_buffers) {

          // Handle errors.  If the error is EOF, then first_.size() will be the
          // remainder of the bytes in the first stream.  Knowing where the end
          // of `first_` is triggers a different handling of a fetch, so record
          // the size in `first_known_available_` and retry.  Otherwise, pass
          // the error on to the client (and we're done).
          //
          if (ec) {
            if (ec == boost::asio::error::eof) {
              assert(!first_known_available_);
              first_known_available_ = first_.size();
              async_fetch(min_count, std::move(captured_handler));
            } else {
              captured_handler(ec, const_buffers_type{});
            }
            return;
          }

          // Save the fetched size for consume later.
          first_fetched_size_ = boost::asio::buffer_size(first_buffers);

          // If this fails, then the underlying stream has broken its contract.
          assert(*first_fetched_size_ >= min_count);

          // Victory!
          captured_handler(
              error_code{},
              const_buffers_type(first_buffers.begin(), first_buffers.end()));
        });
  }

  template <typename Handler>
  void fetch_from_second(const std::size_t min_count, Handler &&handler) {
    second_.async_fetch(
        min_count,
        [ this, captured_handler = std::move(handler) ](
            const error_code &ec,
            const typename Second::const_buffers_type &second_buffers) {
          if (!ec) {
            second_fetched_size_ = boost::asio::buffer_size(second_buffers);
          }
          captured_handler(ec, const_buffers_type(second_buffers.begin(),
                                                  second_buffers.end()));
        });
  }

  template <typename Handler>
  void fetch_from_first_and_second(const std::size_t min_count,
                                   Handler &&handler) {
    countdown_latch_.store(2U, std::memory_order_relaxed);

    first_.async_fetch(
        *first_known_available_,
        [this, handler](const error_code &ec, auto &&first_buffers) {
          first_ec_ = boost::in_place(ec);
          if (!ec) {
            first_fetched_size_ = boost::asio::buffer_size(first_buffers);
            fetched_from_first_ = boost::in_place(std::move(first_buffers));
          }
          handle_concurrent_fetch(std::move(handler));
        });

    second_.async_fetch(
        min_count - *first_known_available_,
        [this, handler](const error_code &ec, auto &&second_buffers) {
          second_ec_ = boost::in_place(ec);
          if (!ec) {
            second_fetched_size_ = boost::asio::buffer_size(second_buffers);
            fetched_from_second_ = boost::in_place(std::move(second_buffers));
          }
          handle_concurrent_fetch(std::move(handler));
        });
  }

  template <typename Handler> void handle_concurrent_fetch(Handler &&handler) {
    // Decrement the latch here; we just need to release to make sure that a
    // possible subsequent acquire fence will pick up all side effects prior to
    // this line.
    if (countdown_latch_.fetch_sub(1, std::memory_order_release) == 1) {
      // The acquire fence here ensures that any side-effects of the other fetch
      // are seen after the fence.
      std::atomic_thread_fence(std::memory_order_acquire);

      if (first_ec_) {
        if (!second_ec_) {
          second_.consume(0);
        } else {
          // If two errors occurred, the first one is reported.
          second_ec_ = boost::none;
        }
        decltype(first_ec_) ec;
        ec.swap(first_ec_);
        handler(*ec, const_buffers_type{});
        return;
      } else if (second_ec_) {
        first_.consume(0);
        decltype(second_ec_) ec;
        ec.swap(second_ec_);
        handler(*ec, const_buffers_type{});
        return;
      }

      assert(!first_ec_ && !second_ec_);
      first_ec_ = boost::none;
      second_ec_ = boost::none;

      const_buffers_type buffers(fetched_from_first_->begin(),
                                 fetched_from_first_->end());

      buffers.insert(buffers.end(), //
                     fetched_from_second_->begin(),
                     fetched_from_second_->end());

      handler(error_code{}, std::move(buffers));
    }
  }

  First first_;
  Second second_;
  boost::optional<std::size_t> first_known_available_;
  boost::optional<error_code> first_ec_;
  boost::optional<error_code> second_ec_;
  boost::optional<std::size_t> first_fetched_size_;
  boost::optional<std::size_t> second_fetched_size_;
  boost::optional<typename First::const_buffers_type> fetched_from_first_;
  boost::optional<typename Second::const_buffers_type> fetched_from_second_;
  std::atomic<unsigned char> countdown_latch_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_CONCAT_CONST_BUFFER_STREAM_H
