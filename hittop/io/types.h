#ifndef HITTOP_IO_TYPES_H
#define HITTOP_IO_TYPES_H

#include "boost/asio/buffer.hpp"
#include "boost/system/error_code.hpp"

namespace hittop {
namespace io {

using mutable_buffer = ::boost::asio::mutable_buffer;
using const_buffer = ::boost::asio::const_buffer;
using error_code = ::boost::system::error_code;

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_TYPES_H
