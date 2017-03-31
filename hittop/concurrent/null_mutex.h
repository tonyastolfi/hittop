#ifndef HITTOP_CONCURRENT_NULL_MUTEX_H
#define HITTOP_CONCURRENT_NULL_MUTEX_H

namespace hittop {
namespace concurrent {

// An implementation of the Lockable concept that performs no synchronization.
struct NullMutex {
  void lock() {}
  void unlock() noexcept {}
  bool try_lock() { return true; }
};

} // namespace concurrent
} // namespace hittop

#endif // HITTOP_CONCURRENT_NULL_MUTEX_H
