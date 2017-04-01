#ifndef HITTOP_IO_BASIC_BUFFER_SEQUENCE_H
#define HITTOP_IO_BASIC_BUFFER_SEQUENCE_H

#include "boost/container/static_vector.hpp"

namespace hittop {
namespace io {

template <typename BufferType, std::size_t MaxSize> class BasicBufferSequence {
  using Storage = boost::container::static_vector<BufferType, MaxSize>;

public:
  using value_type = BufferType;
  using const_iterator = typename Storage::const_iterator;

  template <typename T>
  /* intentionally implicit */ BasicBufferSequence(T &&bs)
      : storage_(bs.begin(), bs.end()) {}

  const_iterator begin() const { return storage_.cbegin(); }
  const_iterator end() const { return storage_.cend(); }

private:
  Storage storage_;
};

} // namespace io
} // namespace hittop

#endif // HITTOP_IO_BASIC_BUFFER_SEQUENCE_H
