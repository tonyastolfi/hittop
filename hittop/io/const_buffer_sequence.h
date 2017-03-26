#ifndef HITTOP_IO_CONST_BUFFER_SEQUENCE_H
#define HITTOP_IO_CONST_BUFFER_SEQUENCE_H

#include "boost/container/static_vector.hpp"

#include "hittop/io/basic_buffer_sequence.h"
#include "hittop/io/types.h"

namespace hittop {
namespace io {

template <std::size_t MaxSize>
using BasicConstBufferSequence = BasicBufferSequence<const_buffer, MaxSize>;

using ConstBufferSequence = BasicConstBufferSequence<4>;

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_CONST_BUFFER_SEQUENCE_H
