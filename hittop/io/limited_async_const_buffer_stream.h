#ifndef HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H
#define HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H

#include <cstddef>
#include <vector>

#include "boost/asio/error.hpp"

#include "hittop/io/async_const_buffer_stream.h"

namespace hittop {
namespace io {

template <typename WrappedStreamType>
class LimitedAsyncConstBufferStream
    : public AsyncConstBufferStream<
          LimitedAsyncConstBufferStream<WrappedStreamType>> {
public:
  using const_buffers_type = std::vector<const_buffer>;

  template <typename... Args>
  explicit LimitedAsyncConstBufferStream(const std::size_t byte_limit,
                                         Args &&... args)
      : bytes_remaining_(byte_limit), args_(std::forward<Args>(args)...) {}

  std::size_t max_size() const {
    return std::min(bytes_remaining_, stream_.max_size());
  }

  std::size_t size() const {
    return std::min(bytes_remaining_, stream_.size());
  }

  template <typename Handler>
  void async_fetch(const std::size_t min_bytes, Handler &&handler) {
    if (min_bytes > bytes_remaining_) {
      handler(boost::asio::error::eof, const_buffers_type{});
    }
    stream_.async_fetch(min_bytes, [
      limit = bytes_remaining_, handler = std::forward<Handler>(handler)
    ](const error_code &ec, const auto &unlimited_buffers) {
      // Push const_buffer objects from unlimited_buffers onto limited_buffers
      // until we run out of data or the limit is reached.
      //
      const_buffers_type limited_buffers;
      std::size_t space_remaining = limit;
      auto next = unlimited_buffers.begin();
      const auto last = unlimited_buffers.end();
      while (space_remaining > 0 && next != last) {
        const std::size_t size = boost::asio::buffer_size(*next);
        if (size > space_remaining) {
          limited_buffers.emplace_back(const_buffer(
              boost::asio::buffer_cast<const void *>(*next), space_remaining));
          break;
        }
        limited_buffers.push_back(*next);
        space_remaining -= size;
        ++next;
      }
      handler(ec, std::move(limited_buffers));
    });
  }

  void consume(const std::size_t count) {
    assert(count <= bytes_remaining_);
    bytes_remaining_ -= count;
    stream_.consume(count);
  }

private:
  std::size_t bytes_remaining_;
  WrappedStreamType stream_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_LIMITED_ASYNC_CONST_BUFFER_STREAM_H
