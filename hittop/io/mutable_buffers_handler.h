#ifndef MUTABLE_BUFFERS_HANDLER_H
#define MUTABLE_BUFFERS_HANDLER_H

#include <functional>

#include "boost/container/static_vector.hpp"

#include "hittop/io/types.h"

namespace hittop {
namespace io {

class MutableBufferSequence {
  using Storage = boost::static_vector<mutable_buffer, 4>;

public:
  using value_type = mutable_buffer;
  using const_iterator = Storage::const_iterator;

  template <typename T>
  /* intentionally implicit */ MutableBufferSequence(T &&mbs)
      : storage_(mbs.begin(), mbs.end()) {}

  const_iterator begin() const { return storage_.cbegin(); }
  const_iterator end() const { return storage_.cend(); }

private:
  Storage storage_;
};

// MutableBufferHandler Concept
//
using MutableBuffersHandler =
    std::function<void(const error_code &, const MutableBufferSequence &)>;

} // namespace io
} // namespace hitttop

#endif // MUTABLE_BUFFERS_HANDLER_H
