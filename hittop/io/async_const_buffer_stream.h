#ifndef HITTOP_IO_ASYNC_CONST_BUFFER_STREAM_H
#define HITTOP_IO_ASYNC_CONST_BUFFER_STREAM_H

#include <type_traits>

#include "hittop/io/const_buffers_handler.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

// AsyncConstBufferStream Concept
//
// Requirements:
//
//  void async_fetch(std::size_t minimum_size, ConstBuffersHandler);
//
// The maximum number of bytes that can be fetched in one call from this stream.
//  std::size_t max_size() const;
//
// The number of bytes which can be fetched right now without waiting.
//  std::size_t size() const;
//
//  void consume(std::size_t byte_count);
//
//  void close_for_read();
//
//  typename AsyncConstBufferStream::const_buffers_type
//     fulfills the requirements of Boost.Asio ConstBufferSequence.
//

template <typename Derived> class AsyncConstBufferStream {
protected:
  AsyncConstBufferStream() {
    static_assert(
        std::is_same<
            void,
            decltype(std::declval<Derived>().async_fetch(
                std::declval<std::size_t>(),
                std::declval<std::function<void(
                    const error_code &,
                    const typename Derived::const_buffers_type &)>>()))>::value,
        "AsyncConstBufferStream must expose a public method "
        "void async_fetch(std::size_t, ConstBuffersHandler)");

    static_assert(
        std::is_same<void, decltype(std::declval<Derived>().async_fetch(
                               std::declval<std::size_t>(),
                               std::declval<ConstBuffersHandler>()))>::value,
        "AsyncConstBufferStream must expose a public method "
        "void async_fetch(std::size_t, ConstBuffersHandler)");

    static_assert(
        std::is_same<std::size_t,
                     decltype(std::declval<const Derived>().max_size())>::value,
        "AsyncConstBufferStream must expose a public method std::size_t "
        "max_size() const");

    static_assert(
        std::is_same<std::size_t,
                     decltype(std::declval<const Derived>().size())>::value,
        "AsyncConstBufferStream must expose a public method std::size_t "
        "size() const");

    static_assert(std::is_same<void, decltype(std::declval<Derived>().consume(
                                         std::declval<std::size_t>()))>::value,
                  "AsyncConstBufferStream must expose a public method void "
                  "consume(std::size_t byte_count)");

    static_assert(
        std::is_same<void,
                     decltype(std::declval<Derived>().close_for_read())>::value,
        "AsyncConstBufferStream must have a public method void "
        "close_for_read()");
  }
};

} // namespace io
} // namespace hitttop

#endif // HITTOP_IO_ASYNC_CONST_BUFFER_STREAM_H
