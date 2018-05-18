#include "hittop/concurrent/async_event.h"
#include "hittop/concurrent/async_event.h"

#include "gtest/gtest.h"

#include "hittop/concurrent/latching_signal.h"

#include <functional>

namespace {

// Test plan:
//  1. Default ctor - check state
//  2. Cleanup function ctor - check state
//  3. Reached -> Then
//  4. Reached -> ThenTC
//  5. ReachedTC -> Then
//  6. ReachedTC -> ThenTC
//  7. Then -> Reached
//  8. Then -> ReachedTC
//  9. ThenTC -> Reached
//  10. ThenTC -> ReachedTC
//  11. Concurrent Then, Reached
//  12. Concurrent Then, ReachedTC
//  13. Concurrent ThenTC, Reached
//  14. Concurrent ThenTC, ReachedTC

using hittop::concurrent::AsyncEvent;
using MockCleanup = hittop::concurrent::LatchingSignal<void()>;
using MockListener = hittop::concurrent::LatchingSignal<void()>;

class AsyncEventTest : public testing::Test {
protected:
  MockCleanup cleanup_;
  MockListener listener_;
  AsyncEvent event_{std::ref(cleanup_)};
};

TEST_F(AsyncEventTest, DefaultCtor) {
  AsyncEvent e;
  EXPECT_EQ(AsyncEvent::kInitial, e.state());
}

TEST_F(AsyncEventTest, CleanupCtor) {
  EXPECT_EQ(AsyncEvent::kInitial, event_.state());
  EXPECT_FALSE(cleanup_.is_latched());
  EXPECT_FALSE(listener_.is_latched());
}

TEST_F(AsyncEventTest, ReachedThen) {
  event_.Reached();

  EXPECT_FALSE(cleanup_.is_latched());
  EXPECT_FALSE(listener_.is_latched());

  bool cleanup_before_listener = false;
  auto connection = cleanup_.Connect(
      [&]() { cleanup_before_listener = !listener_.is_latched(); });

  EXPECT_TRUE(!!connection);

  event_.Then(std::ref(listener_));

  EXPECT_TRUE(cleanup_.is_latched());
  EXPECT_TRUE(listener_.is_latched());
  EXPECT_TRUE(cleanup_before_listener);
}

TEST_F(AsyncEventTest, ReachedThenTC) {}
TEST_F(AsyncEventTest, ReachedTCThen) {}
TEST_F(AsyncEventTest, ReachedTCThenTC) {}
TEST_F(AsyncEventTest, ThenReached) {}
TEST_F(AsyncEventTest, ThenReachedTC) {}
TEST_F(AsyncEventTest, ThenTCReached) {}
TEST_F(AsyncEventTest, ThenTCReachedTC) {}
TEST_F(AsyncEventTest, ConcurrentReachedThen) {}
TEST_F(AsyncEventTest, ConcurrentReachedThenTC) {}
TEST_F(AsyncEventTest, ConcurrentReachedTCThen) {}
TEST_F(AsyncEventTest, ConcurrentReachedTCThenTC) {}

} // namespace
