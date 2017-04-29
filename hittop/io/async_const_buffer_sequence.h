#ifndef HITTOP_IO_ASYNC_CONST_BUFFER_SEQUENCE_H
#define HITTOP_IO_ASYNC_CONST_BUFFER_SEQUENCE_H

#include <assert.h>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/asio/error.hpp"

#include "hittop/io/async_const_buffer_stream.h"

namespace hittop {
namespace io {

template <typename ConstBufferSequence>
class AsyncConstBufferSequence final
    : public AsyncConstBufferStream<
          AsyncConstBufferSequence<ConstBufferSequence>> {
public:
  using const_buffers_type = std::vector<const_buffer>;

  template <typename... Args>
  explicit AsyncConstBufferSequence(Args &&... args)
      : buffers_(std::forward<Args>(args)...) {}

  std::size_t max_size() const { return available_; }

  std::size_t size() const { return available_; }

  template <typename Handler>
  void async_fetch(std::size_t min_bytes, Handler &&handler) const {
    if (min_bytes > available_) {
      std::forward<Handler>(handler)(boost::asio::error::eof,
                                     const_buffers_type{});
      return;
    }
    const_buffers_type bs(next_, last_);
    if (!bs.empty()) {
      bs.front() = bs.front() + offset_;
    }
    std::forward<Handler>(handler)({}, std::move(bs));
  }

  void consume(std::size_t count) {
    assert(count <= available_);
    available_ -= count;
    while (count > 0) {
      const std::size_t remaining_this_buffer = buffer_size(*next_) - offset_;
      if (count >= remaining_this_buffer) {
        ++next_;
        offset_ = 0;
        count -= remaining_this_buffer;
      } else {
        offset_ += count;
        break;
      }
    }
  }

  void close_for_read() const {} // nothing really to do here.

private:
  using Iterator = decltype(std::declval<ConstBufferSequence>().begin());

  ConstBufferSequence buffers_;
  std::size_t available_ = boost::asio::buffer_size(buffers_);
  Iterator next_{buffers_.begin()};
  std::size_t offset_ = 0;
  const Iterator last_{buffers_.end()};
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_ASYNC_CONST_BUFFER_SEQUENCE_H
