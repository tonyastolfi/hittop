#ifndef HITTOP_IO_ASYNC_MUTABLE_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_MUTABLE_BUFFER_STREAM_H

#include <type_traits>

#include "hittop/io/mutable_buffers_handler.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

// AsyncMutableBufferStream Concept
//
// Requirements:
//
//  void async_prepare(std::size_t minimum_size, MutableBuffersHandler);
//
//  std::size_t max_space() const;
//
//  std::size_t space() const;
//
//  void commit(std::size_t byte_count);
//
//  void close_for_write();
//
//  typename AsyncMutableBufferStream::mutable_buffers_type
//     fulfills the requirements of Boost.Asio MutableBufferSequence.
//

template <typename Derived> class AsyncMutableBufferStream {
protected:
  AsyncMutableBufferStream() {
    static_assert(
        std::is_same<void, decltype(std::declval<Derived>().async_prepare(
                               std::declval<std::size_t>(),
                               std::declval<std::function<void(
                                   const error_code &,
                                   const typename Derived::mutable_buffers_type
                                       &)>>()))>::value,
        "AsyncMutableBufferStream must expose a public method "
        "void async_prepare(std::size_t, MutableBuffersHandler)");

    static_assert(
        std::is_same<void, decltype(std::declval<Derived>().async_prepare(
                               std::declval<std::size_t>(),
                               std::declval<MutableBuffersHandler>()))>::value,
        "AsyncMutableBufferStream must expose a public method "
        "void async_prepare(std::size_t, MutableBuffersHandler)");

    static_assert(
        std::is_same<std::size_t, decltype(std::declval<const Derived>()
                                               .max_space())>::value,
        "AsyncMutableBufferStream must expose a public method std::size_t "
        "max_space() const");

    static_assert(
        std::is_same<std::size_t,
                     decltype(std::declval<const Derived>().space())>::value,
        "AsyncMutableBufferStream must expose a public method std::size_t "
        "space() const");

    static_assert(std::is_same<void, decltype(std::declval<Derived>().commit(
                                         std::declval<std::size_t>()))>::value,
                  "AsyncMutableBufferStream must expose a public method void "
                  "commit(std::size_t byte_count)");

    static_assert(std::is_same<void, decltype(std::declval<Derived>()
                                                  .close_for_write())>::value,
                  "AsyncMutableBufferStream must have a public method void "
                  "close_for_write()");
  }
};

} // namespace io
} // namespace hitttop

#endif // HITTOP_IO_ASYNC_MUTABLE_BUFFER_STREAM_H
