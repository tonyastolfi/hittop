#ifndef HITTOP_IO_CIRCULAR_BUFFER_H
#define HITTOP_IO_CIRCULAR_BUFFER_H

#include <assert.h>

#include <atomic>
#include <cstddef>
#include <vector>

#include "boost/asio/buffer.hpp"
#include "boost/container/static_vector.hpp"
#include "boost/system/error_code.hpp"

#include "hittop/io/types.h"

namespace hittop {
namespace io {

template <typename IntType> class BasicCircularBuffer {
public:
  using mutable_buffers_type = //
      boost::container::static_vector<mutable_buffer, 2>;

  using const_buffers_type = //
      boost::container::static_vector<const_buffer, 2>;

  explicit BasicCircularBuffer(const int size_log_2)
      : storage_(1 << size_log_2) {
    read_head_ = 0;
    write_head_ = 0;
  }

  BasicCircularBuffer(const BasicCircularBuffer &) = delete;
  BasicCircularBuffer &operator=(const BasicCircularBuffer &) = delete;

  // ===========================================================================
  // Invariant interface
  // ===========================================================================

  std::size_t max_size() const { return storage_.size(); }

  // ===========================================================================
  // Reader interface
  // ===========================================================================

  // Return a ConstBufferSequence containing valid bytes for reading.
  //
  // Thread safety: The "true" data region (as updated by write commits) is
  // guaranteed to be equal to or larger than the region returned by this
  // method; further, calls to prepare/commit do not invalidate the returned
  // buffers.  Calls to data() and consume() must be serialized by the caller.
  //
  const_buffers_type data() const {

    // Thread safey note for future maintainers: this method does a single load
    // from a concurrently updated variable, the `write_head_`.  Because the
    // size of the readable region increases with increases in `write_head_`
    // value and `write_head_` increases monotonically in atomic steps, this
    // guarantees the thread safety properties stated above.
    //
    assert(write_head_ - read_head_ <= storage_.size());
    return GetRange<const_buffer>(read_head_ & mask_, size(), storage_);
  }

  void consume(const std::size_t byte_count) {
    assert(byte_count <= size());
    read_head_ += byte_count;
  }

  // ===========================================================================
  // Writer interface
  // ===========================================================================

  // Return a MutableBufferSequence containing valid bytes for writing.
  //
  // Thread safety: The "true" prepared region (as updated by consume) is
  // guaranteed to be equal to or larger than the region returned by this
  // method; further, calls to data/consume do not invalidate the returned
  // buffers.  Calls to prepare() and commit() must be serialized by the caller.
  //
  mutable_buffers_type prepare() {
    assert(write_head_ - read_head_ <= storage_.size());
    return GetRange<mutable_buffer>(write_head_ & mask_, space(), storage_);
  }

  void commit(const std::size_t byte_count) {
    assert(byte_count <= space());
    write_head_ += byte_count;
  }

  // ===========================================================================
  // Shared interface
  // ===========================================================================

  // The number of bytes available for reading.
  //
  // Writers increase (commit), readers decrease (consume).
  //
  std::size_t size() const { return write_head_ - read_head_; }

  // The number of bytes available for writing.
  //
  // Readers increase (consume), writers decrease (commit).
  //
  std::size_t space() const { return max_size() - size(); }

  // true iff there are no bytes available to read; if true, then max_size()
  // bytes are available for writing.
  //
  // Readers change from false to true; writers change from true to false.
  //
  bool empty() const { return read_head_ == write_head_; }

  // true iff there are no bytes available for writing; if true, then size() ==
  // max_size().
  //
  // Writers change from false to true; readers change from true to false.
  //
  bool full() const { return write_head_ - read_head_ == storage_.size(); }

  // Dump the buffer state (human-readable); for debugging.  Does not write the
  // contents of the buffer, only the read/write head positions.
  //
  friend std::ostream &operator<<(std::ostream &os,
                                  const BasicCircularBuffer &b) {
    return os << "buffer(read=" << (b.read_head_ & b.mask_)
              << ", write=" << (b.write_head_ & b.mask_) << ")";
  }

private:
  template <typename Buffer, typename Storage>
  static boost::container::static_vector<Buffer, 2>
  GetRange(const std::size_t offset, const std::size_t count,
           Storage &storage) {
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
  const std::size_t mask_ = storage_.size() - 1;
  IntType read_head_;
  IntType write_head_; // stays ahead of read start
};

using CircularBuffer = BasicCircularBuffer<std::atomic<size_t>>;

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_CIRCULAR_BUFFER_H
