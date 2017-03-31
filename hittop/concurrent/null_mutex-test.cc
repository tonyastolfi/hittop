#include "hittop/concurrent/null_mutex.h"
#include "hittop/concurrent/null_mutex.h"

#include "gtest/gtest.h"

#include <mutex>

namespace {

TEST(NullMutexTest, LockUnlock) {
  hittop::concurrent::NullMutex m;
  std::unique_lock<hittop::concurrent::NullMutex> lock(m);
}

} // namespace
