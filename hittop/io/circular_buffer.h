#ifndef HITTOP_IO_CIRCULAR_BUFFER_H
#define HITTOP_IO_CIRCULAR_BUFFER_H

#include <assert.h>

#include <cstddef>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/container/static_vector.hpp"
#include "boost/system/error_code.hpp"

namespace hittop {
namespace io {

class CircularBuffer {
public:
  using mutable_buffers_type = //
      boost::container::static_vector<mutable_buffer, 2>;

  using const_buffers_type = //
      boost::container::static_vector<const_buffer, 2>;

  explicit CircularBuffer(const int size_log_2) : storage_(1 << size_log_2) {}

  CircularBuffer(const CircularBuffer &) = delete;
  CircularBuffer &operator=(const CircularBuffer &) = delete;

  void commit(const std::size_t byte_count) {
    assert(byte_count <= space());
    write_head_ += byte_count;
  }

  void consume(const std::size_t byte_count) {
    assert(byte_count <= size());
    read_head_ += byte_count;
  }

  std::size_t max_size() const { return storage_.size(); }

  std::size_t size() const { return write_head_ - read_head_; }

  std::size_t space() const { return max_size() - size(); }

  const_buffers_type data() const {
    assert(write_head_ - read_head_ <= storage_.size());
    return GetRange<const_buffer>(read_head_ & mask_, size(), storage_);
  }

  mutable_buffers_type prepare() {
    assert(write_head_ - read_head_ <= storage_.size());
    return GetRange<mutable_buffer>(write_head_ & mask_, space(), storage_);
  }

  bool empty() const { return read_head_ == write_head_; }

  bool full() const { return write_head_ - read_head_ == storage_.size(); }

  friend std::ostream &operator<<(std::ostream &os, const CircularBuffer &b) {
    return os << "buffer(read=" << (b.read_head_ & b.mask_)
              << ", write=" << (b.write_head_ & b.mask_) << ")";
  }

private:
  template <typename Buffer, typename Storage>
  static boost::container::static_vector<Buffer, 2>
  GetRange(std::size_t offset, std::size_t count, Storage &storage) {
    assert(count <= storage.size());
    // When the end of the returned range is before the end of the buffer, just
    // return a single segment.
    if (offset + count <= storage.size()) {
      return {boost::asio::buffer(&storage[offset], count)};
    } else {
      // When the end of the returned range is after the physical end of the
      // buffer, then we must wrap around by returning two segments.
      return {
          boost::asio::buffer(&storage[offset], storage.size() - offset),
          boost::asio::buffer(&storage[0], offset + count - storage.size())};
    }
  }

  std::vector<char> storage_;
  std::size_t mask_ = storage_.size() - 1;
  std::size_t read_head_ = 0;
  std::size_t write_head_ = 0; // stays ahead of read start
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_CIRCULAR_BUFFER_H
